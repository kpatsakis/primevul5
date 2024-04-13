    bool writeSource (OutputStream& target)
    {
        if (stream == nullptr)
        {
            stream = file.createInputStream();

            if (stream == nullptr)
                return false;
        }

        checksum = 0;
        uncompressedSize = 0;
        const int bufferSize = 4096;
        HeapBlock<unsigned char> buffer (bufferSize);

        while (! stream->isExhausted())
        {
            auto bytesRead = stream->read (buffer, bufferSize);

            if (bytesRead < 0)
                return false;

            checksum = zlibNamespace::crc32 (checksum, buffer, (unsigned int) bytesRead);
            target.write (buffer, (size_t) bytesRead);
            uncompressedSize += bytesRead;
        }

        stream.reset();
        return true;
    }