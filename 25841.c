Result ZipFile::uncompressTo (const File& targetDirectory,
                              const bool shouldOverwriteFiles)
{
    for (int i = 0; i < entries.size(); ++i)
    {
        auto result = uncompressEntry (i, targetDirectory, shouldOverwriteFiles);

        if (result.failed())
            return result;
    }

    return Result::ok();
}