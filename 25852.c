    bool setPosition (int64 newPos) override
    {
        pos = jlimit ((int64) 0, zipEntryHolder.compressedSize, newPos);
        return true;
    }