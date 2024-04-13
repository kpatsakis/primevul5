    ~ZipInputStream() override
    {
       #if JUCE_DEBUG
        if (inputStream != nullptr && inputStream == file.inputStream)
            file.streamCounter.numOpenStreams--;
       #endif
    }