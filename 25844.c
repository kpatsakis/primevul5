    ZipInputStream (ZipFile& zf, const ZipFile::ZipEntryHolder& zei)
        : file (zf),
          zipEntryHolder (zei),
          inputStream (zf.inputStream)
    {
        if (zf.inputSource != nullptr)
        {
            streamToDelete.reset (file.inputSource->createInputStream());
            inputStream = streamToDelete.get();
        }
        else
        {
           #if JUCE_DEBUG
            zf.streamCounter.numOpenStreams++;
           #endif
        }

        char buffer[30];

        if (inputStream != nullptr
             && inputStream->setPosition (zei.streamOffset)
             && inputStream->read (buffer, 30) == 30
             && ByteOrder::littleEndianInt (buffer) == 0x04034b50)
        {
            headerSize = 30 + ByteOrder::littleEndianShort (buffer + 26)
                            + ByteOrder::littleEndianShort (buffer + 28);
        }
    }