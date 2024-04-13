    ZipEntryHolder (const char* buffer, int fileNameLen)
    {
        isCompressed           = readUnalignedLittleEndianShort (buffer + 10) != 0;
        entry.fileTime         = parseFileTime (readUnalignedLittleEndianShort (buffer + 12),
                                                readUnalignedLittleEndianShort (buffer + 14));
        compressedSize         = (int64) readUnalignedLittleEndianInt (buffer + 20);
        entry.uncompressedSize = (int64) readUnalignedLittleEndianInt (buffer + 24);
        streamOffset           = (int64) readUnalignedLittleEndianInt (buffer + 42);

        entry.externalFileAttributes = readUnalignedLittleEndianInt (buffer + 38);
        auto fileType = (entry.externalFileAttributes >> 28) & 0xf;
        entry.isSymbolicLink = (fileType == 0xA);

        entry.filename = String::fromUTF8 (buffer + 46, fileNameLen);
    }