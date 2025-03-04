Result ZipFile::uncompressEntry (int index, const File& targetDirectory, OverwriteFiles overwriteFiles, FollowSymlinks followSymlinks)
{
    auto* zei = entries.getUnchecked (index);

   #if JUCE_WINDOWS
    auto entryPath = zei->entry.filename;
   #else
    auto entryPath = zei->entry.filename.replaceCharacter ('\\', '/');
   #endif

    if (entryPath.isEmpty())
        return Result::ok();

    auto targetFile = targetDirectory.getChildFile (entryPath);

    if (! targetFile.isAChildOf (targetDirectory))
        return Result::fail ("Entry " + entryPath + " is outside the target directory");

    if (entryPath.endsWithChar ('/') || entryPath.endsWithChar ('\\'))
        return targetFile.createDirectory(); // (entry is a directory, not a file)

    std::unique_ptr<InputStream> in (createStreamForEntry (index));

    if (in == nullptr)
        return Result::fail ("Failed to open the zip file for reading");

    if (targetFile.exists())
    {
        if (overwriteFiles == OverwriteFiles::no)
            return Result::ok();

        if (! targetFile.deleteFile())
            return Result::fail ("Failed to write to target file: " + targetFile.getFullPathName());
    }

    if (followSymlinks == FollowSymlinks::no && hasSymbolicPart (targetDirectory, targetFile.getParentDirectory()))
        return Result::fail ("Parent directory leads through symlink for target file: " + targetFile.getFullPathName());

    if (! targetFile.getParentDirectory().createDirectory())
        return Result::fail ("Failed to create target folder: " + targetFile.getParentDirectory().getFullPathName());

    if (zei->entry.isSymbolicLink)
    {
        String originalFilePath (in->readEntireStreamAsString()
                                    .replaceCharacter (L'/', File::getSeparatorChar()));

        if (! File::createSymbolicLink (targetFile, originalFilePath, true))
            return Result::fail ("Failed to create symbolic link: " + originalFilePath);
    }
    else
    {
        FileOutputStream out (targetFile);

        if (out.failedToOpen())
            return Result::fail ("Failed to write to target file: " + targetFile.getFullPathName());

        out << *in;
    }

    targetFile.setCreationTime (zei->entry.fileTime);
    targetFile.setLastModificationTime (zei->entry.fileTime);
    targetFile.setLastAccessTime (zei->entry.fileTime);

    return Result::ok();
}