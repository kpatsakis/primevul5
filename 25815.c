InputStream* ZipFile::createStreamForEntry (const int index)
{
    InputStream* stream = nullptr;

    if (auto* zei = entries[index])
    {
        stream = new ZipInputStream (*this, *zei);

        if (zei->isCompressed)
        {
            stream = new GZIPDecompressorInputStream (stream, true,
                                                      GZIPDecompressorInputStream::deflateFormat,
                                                      zei->entry.uncompressedSize);

            // (much faster to unzip in big blocks using a buffer..)
            stream = new BufferedInputStream (stream, 32768, true);
        }
    }

    return stream;
}