int ZipFile::getIndexOfFileName (const String& fileName, bool ignoreCase) const noexcept
{
    for (int i = 0; i < entries.size(); ++i)
    {
        auto& entryFilename = entries.getUnchecked (i)->entry.filename;

        if (ignoreCase ? entryFilename.equalsIgnoreCase (fileName)
                       : entryFilename == fileName)
            return i;
    }

    return -1;
}