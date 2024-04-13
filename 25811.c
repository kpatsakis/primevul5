Result ZipFile::uncompressEntry (int index, const File& targetDirectory, bool shouldOverwriteFiles)
{
    return uncompressEntry (index,
                            targetDirectory,
                            shouldOverwriteFiles ? OverwriteFiles::yes : OverwriteFiles::no,
                            FollowSymlinks::no);
}