    bool writeData (OutputStream& target, const int64 overallStartPosition)
    {
        MemoryOutputStream compressedData ((size_t) file.getSize());

        if (symbolicLink)
        {
            auto relativePath = file.getNativeLinkedTarget().replaceCharacter (File::getSeparatorChar(), L'/');

            uncompressedSize = relativePath.length();

            checksum = zlibNamespace::crc32 (0, (uint8_t*) relativePath.toRawUTF8(), (unsigned int) uncompressedSize);
            compressedData << relativePath;
        }
        else if (compressionLevel > 0)
        {
            GZIPCompressorOutputStream compressor (compressedData, compressionLevel,
                                                   GZIPCompressorOutputStream::windowBitsRaw);
            if (! writeSource (compressor))
                return false;
        }
        else
        {
            if (! writeSource (compressedData))
                return false;
        }

        compressedSize = (int64) compressedData.getDataSize();
        headerStart = target.getPosition() - overallStartPosition;

        target.writeInt (0x04034b50);
        writeFlagsAndSizes (target);
        target << storedPathname
               << compressedData;

        return true;
    }