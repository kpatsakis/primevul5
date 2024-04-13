const ZipFile::ZipEntry* ZipFile::getEntry (const String& fileName, bool ignoreCase) const noexcept
{
    return getEntry (getIndexOfFileName (fileName, ignoreCase));
}