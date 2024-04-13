const ZipFile::ZipEntry* ZipFile::getEntry (const int index) const noexcept
{
    if (auto* zei = entries[index])
        return &(zei->entry);

    return nullptr;
}