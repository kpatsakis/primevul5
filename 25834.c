    bool isExhausted() override
    {
        return headerSize <= 0 || pos >= zipEntryHolder.compressedSize;
    }