// ============================================================================
// FileLoader.cpp
// ============================================================================
#include "FileLoader.h"

FileLoader::FileLoader()
{
    // Register audio formats
    formatManager.registerBasicFormats();

    // Setup load button
    loadFilesButton.setButtonText("Load Audio Files");
    loadFilesButton.onClick = [this] { showSampleSelectionMenu(); };
    addAndMakeVisible(loadFilesButton);

    // Setup status label
    statusLabel.setText("No files loaded", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(statusLabel);
}

FileLoader::~FileLoader()
{
}

void FileLoader::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // === OUTER SHADOW ===
    juce::Path outerPath;
    outerPath.addRoundedRectangle(bounds, 8.0f);

    juce::DropShadow shadow(
        juce::Colour(0xFF000000).withAlpha(0.6f),
        8,
        { 0, 2 }
    );
    shadow.drawForPath(g, outerPath);

    // === MAIN BACKGROUND GRADIENT ===
    juce::ColourGradient bg(
        juce::Colour(0xFF1A1A2E), bounds.getX(), bounds.getY(),
        juce::Colour(0xFF16213E), bounds.getX(), bounds.getBottom(),
        false
    );
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, 8.0f);

    // === INNER CYAN GLOW ===
    juce::ColourGradient innerGlow(
        juce::Colour(0xFF4FCFFF).withAlpha(0.15f),
        bounds.getCentreX(), bounds.getY(),
        juce::Colours::transparentBlack,
        bounds.getCentreX(), bounds.getY() + 60.0f,
        false
    );
    g.setGradientFill(innerGlow);
    g.fillRoundedRectangle(bounds.reduced(1.0f), 7.0f);

    // === GLASS REFLECTION ===
    auto reflectionArea = bounds
        .removeFromTop(bounds.getHeight() * 0.3f)
        .reduced(8.0f, 4.0f);

    juce::ColourGradient reflection(
        juce::Colours::white.withAlpha(0.1f),
        reflectionArea.getCentreX(), reflectionArea.getY(),
        juce::Colours::transparentWhite,
        reflectionArea.getCentreX(), reflectionArea.getBottom(),
        false
    );
    g.setGradientFill(reflection);
    g.fillRoundedRectangle(reflectionArea, 4.0f);

    // === TITLE ===
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));

    auto titleBounds = getLocalBounds().removeFromTop(35).reduced(5);
    g.drawText("Sample Loader", titleBounds, juce::Justification::centred);

    // === BLUE UNDERLINE ===
    g.setColour(juce::Colour(0xFF6FCFFF).withAlpha(0.6f));
    float lineY = titleBounds.getBottom() + 5.0f;
    g.drawLine(
        titleBounds.getX() + 10.0f,
        lineY,
        titleBounds.getRight() - 10.0f,
        lineY,
        2.0f
    );
}

void FileLoader::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Space for title
    const int titleHeight = 35;
    bounds.removeFromTop(titleHeight + 6);

    // Split layout
    const int controlWidth = juce::jlimit(140, 180, bounds.getWidth() / 3);
    auto controlArea = bounds.removeFromLeft(controlWidth);
    auto statusArea = bounds;

    // === LEFT COLUMN ===
    const int buttonHeight = 30;
    const int gap = (controlArea.getHeight() - buttonHeight) / 2;

    loadFilesButton.setBounds(
        controlArea.getX() + 4,
        controlArea.getY() + gap,
        controlArea.getWidth() - 8,
        buttonHeight
    );

    // === RIGHT COLUMN ===
    statusLabel.setBounds(statusArea.reduced(6));
    statusLabel.setJustificationType(juce::Justification::centred);
}

void FileLoader::registerPlayer(const juce::String& name, AudioSamplePlayer* player)
{
    if (player != nullptr)
    {
        samplePlayers[name] = player;
    }
}

bool FileLoader::loadFileForPlayer(const juce::String& playerName, const juce::File& file)
{
    // Find the player
    auto it = samplePlayers.find(playerName);
    if (it == samplePlayers.end())
    {
        DBG("Player not found: " + playerName);
        return false;
    }

    // Check if file exists
    if (!file.existsAsFile())
    {
        DBG("File does not exist: " + file.getFullPathName());
        return false;
    }

    AudioSamplePlayer* player = it->second;

    juce::AudioBuffer<float> preparedBuffer;
    double sourceSampleRate = 0.0;

    // Load and prepare the file
    if (loadAndPrepareFile(file, preparedBuffer, sourceSampleRate))
    {
        // Pass the buffer to the player
        if (player->loadBuffer(preparedBuffer, sourceSampleRate))
        {
            statusLabel.setText(playerName + ": " + file.getFileName(),
                juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
            DBG("Successfully loaded " + file.getFullPathName() + " for " + playerName);
            return true;
        }
        else
        {
            DBG("Failed to load buffer for " + playerName);
            return false;
        }
    }
    else
    {
        DBG("Failed to read/prepare file: " + file.getFullPathName());
        return false;
    }
}

bool FileLoader::loadAndPrepareFile(const juce::File& file,
    juce::AudioBuffer<float>& outBuffer,
    double& outSourceSampleRate)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader == nullptr)
    {
        DBG("Failed to create reader for file: " + file.getFullPathName());
        return false;
    }

    std::unique_ptr<juce::AudioFormatReader> readerPtr(reader);

    // Store the original sample rate
    outSourceSampleRate = reader->sampleRate;

    // Validate sample rate
    if (currentSampleRate <= 0.0)
    {
        DBG("Warning: currentSampleRate not set, defaulting to 44100 Hz");
        currentSampleRate = 44100.0;
    }

    // Check if we need resampling
    if (std::abs(outSourceSampleRate - currentSampleRate) > 0.1)
    {
        // Calculate resampled length
        double ratio = currentSampleRate / outSourceSampleRate;
        int resampledLength = static_cast<int>(std::ceil(reader->lengthInSamples * ratio));

        // Add safety margin to prevent buffer overruns
        resampledLength += 16;

        // Load original samples into temporary buffer
        juce::AudioBuffer<float> tempBuffer;
        tempBuffer.setSize(static_cast<int>(reader->numChannels),
            static_cast<int>(reader->lengthInSamples));

        if (!reader->read(&tempBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true))
        {
            DBG("Failed to read audio data from file");
            return false;
        }

        // Resample to match current sample rate
        outBuffer.setSize(static_cast<int>(reader->numChannels), resampledLength);
        outBuffer.clear();

        juce::LagrangeInterpolator interpolator;
        for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel)
        {
            interpolator.reset();
            int samplesWritten = interpolator.process(ratio,
                tempBuffer.getReadPointer(channel),
                outBuffer.getWritePointer(channel),
                resampledLength - 16);

            DBG("Channel " + juce::String(channel) + " wrote " + juce::String(samplesWritten) + " samples");
        }

        DBG("Resampled audio from " + juce::String(outSourceSampleRate) +
            " Hz to " + juce::String(currentSampleRate) + " Hz");
        DBG("Original length: " + juce::String(reader->lengthInSamples) +
            ", Resampled length: " + juce::String(resampledLength));

        // Update the source sample rate to reflect the resampled rate
        // This is what AudioSamplePlayer will store
        outSourceSampleRate = currentSampleRate;
    }
    else
    {
        // No resampling needed - direct load
        outBuffer.setSize(static_cast<int>(reader->numChannels),
            static_cast<int>(reader->lengthInSamples));

        if (!reader->read(&outBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true))
        {
            DBG("Failed to read audio data from file");
            return false;
        }

        DBG("Loaded audio directly at " + juce::String(outSourceSampleRate) + " Hz, " +
            juce::String(outBuffer.getNumSamples()) + " samples");
    }

    return true;
}

void FileLoader::showSampleSelectionMenu()
{
    juce::PopupMenu menu;

    // Add menu items for each registered sample player
    int itemID = 1;
    std::map<int, juce::String> idToName;

    for (const auto& pair : samplePlayers)
    {
        menu.addItem(itemID, "Load " + pair.first);
        idToName[itemID] = pair.first;
        itemID++;
    }

    menu.addSeparator();
    menu.addItem(itemID, "Load All Samples");
    const int loadAllID = itemID;

    // Show menu and handle selection
    menu.showMenuAsync(juce::PopupMenu::Options(),
        [this, idToName, loadAllID](int result)
        {
            if (result == 0)
                return; // User cancelled

            if (result == loadAllID)
            {
                // Load all samples
                loadFilesButtonClicked();
            }
            else
            {
                // Load specific sample
                auto it = idToName.find(result);
                if (it != idToName.end())
                {
                    const juce::String& sampleName = it->second;
                    AudioSamplePlayer* player = samplePlayers[sampleName];

                    // Open file chooser for this specific sample
                    auto fileChooser = std::make_shared<juce::FileChooser>(
                        "Select audio file for " + sampleName,
                        juce::File{},
                        "*.wav;*.mp3;*.aif;*.aiff;*.flac;*.ogg"
                    );

                    auto chooserFlags = juce::FileBrowserComponent::openMode
                        | juce::FileBrowserComponent::canSelectFiles;

                    fileChooser->launchAsync(chooserFlags,
                        [this, player, sampleName, fileChooser](const juce::FileChooser& fc)
                        {
                            auto file = fc.getResult();

                            if (file != juce::File{})
                            {
                                juce::AudioBuffer<float> preparedBuffer;
                                double sourceSampleRate = 0.0;

                                if (loadAndPrepareFile(file, preparedBuffer, sourceSampleRate))
                                {
                                    if (player->loadBuffer(preparedBuffer, sourceSampleRate))
                                    {
                                        statusLabel.setText(sampleName + ": " + file.getFileName(),
                                            juce::dontSendNotification);
                                        statusLabel.setColour(juce::Label::textColourId,
                                            juce::Colours::lightgreen);
                                        DBG(sampleName + " loaded: " + file.getFullPathName());
                                    }
                                    else
                                    {
                                        statusLabel.setText("Failed to load buffer for " + sampleName,
                                            juce::dontSendNotification);
                                        statusLabel.setColour(juce::Label::textColourId,
                                            juce::Colours::red);
                                    }
                                }
                                else
                                {
                                    statusLabel.setText("Failed to read file for " + sampleName,
                                        juce::dontSendNotification);
                                    statusLabel.setColour(juce::Label::textColourId,
                                        juce::Colours::red);
                                }
                            }
                        });
                }
            }
        });
}

void FileLoader::loadFilesButtonClicked()
{
    // Create a vector of player names in order for sequential loading
    std::vector<juce::String> playerNames;
    for (const auto& pair : samplePlayers)
    {
        playerNames.push_back(pair.first);
    }

    // Start with the first player
    loadNextSample(playerNames, 0, std::make_shared<int>(0));
}

void FileLoader::loadNextSample(const std::vector<juce::String>& playerNames,
    int currentIndex,
    std::shared_ptr<int> totalLoaded)
{
    if (currentIndex >= playerNames.size())
    {
        // All done!
        if (*totalLoaded > 0)
        {
            statusLabel.setText(juce::String(*totalLoaded) + " file(s) loaded successfully",
                juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
        }
        return;
    }

    const juce::String& currentPlayerName = playerNames[currentIndex];
    AudioSamplePlayer* currentPlayer = samplePlayers[currentPlayerName];

    auto fileChooser = std::make_shared<juce::FileChooser>(
        "Select audio file for " + currentPlayerName,
        juce::File{},
        "*.wav;*.mp3;*.aif;*.aiff;*.flac;*.ogg"
    );

    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(chooserFlags,
        [this, fileChooser, playerNames, currentIndex, totalLoaded, currentPlayer, currentPlayerName]
        (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                juce::AudioBuffer<float> preparedBuffer;
                double sourceSampleRate = 0.0;

                if (loadAndPrepareFile(file, preparedBuffer, sourceSampleRate))
                {
                    if (currentPlayer->loadBuffer(preparedBuffer, sourceSampleRate))
                    {
                        (*totalLoaded)++;
                        statusLabel.setText(currentPlayerName + ": " + file.getFileName(),
                            juce::dontSendNotification);
                        statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
                        DBG(currentPlayerName + " loaded: " + file.getFullPathName());
                    }
                    else
                    {
                        statusLabel.setText("Failed to load buffer for " + currentPlayerName,
                            juce::dontSendNotification);
                        statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
                    }
                }
                else
                {
                    statusLabel.setText("Failed to read file for " + currentPlayerName,
                        juce::dontSendNotification);
                    statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
                }
            }

            // Move to next sample (whether current succeeded or user cancelled)
            loadNextSample(playerNames, currentIndex + 1, totalLoaded);
        });
}