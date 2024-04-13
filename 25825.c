InputStream* ZipFile::createStreamForEntry (const ZipEntry& entry)
{
    for (int i = 0; i < entries.size(); ++i)
        if (&entries.getUnchecked (i)->entry == &entry)
            return createStreamForEntry (i);

    return nullptr;
}