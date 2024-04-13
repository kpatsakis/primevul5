void ZipFile::Builder::addFile (const File& file, int compression, const String& path)
{
    items.add (new Item (file, nullptr, compression,
                         path.isEmpty() ? file.getFileName() : path,
                         file.getLastModificationTime()));
}