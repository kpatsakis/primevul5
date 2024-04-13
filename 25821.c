    int read (void* buffer, int howMany) override
    {
        if (headerSize <= 0)
            return 0;

        howMany = (int) jmin ((int64) howMany, zipEntryHolder.compressedSize - pos);

        if (inputStream == nullptr)
            return 0;

        int num;

        if (inputStream == file.inputStream)
        {
            const ScopedLock sl (file.lock);
            inputStream->setPosition (pos + zipEntryHolder.streamOffset + headerSize);
            num = inputStream->read (buffer, howMany);
        }
        else
        {
            inputStream->setPosition (pos + zipEntryHolder.streamOffset + headerSize);
            num = inputStream->read (buffer, howMany);
        }

        pos += num;
        return num;
    }