    Item (const File& f, InputStream* s, int compression, const String& storedPath, Time time)
        : file (f), stream (s), storedPathname (storedPath), fileTime (time), compressionLevel (compression)
    {
        symbolicLink = (file.exists() && file.isSymbolicLink());
    }