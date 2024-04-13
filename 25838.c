void ZipFile::Builder::addEntry (InputStream* stream, int compression, const String& path, Time time)
{
    jassert (stream != nullptr); // must not be null!
    jassert (path.isNotEmpty());
    items.add (new Item ({}, stream, compression, path, time));
}