bool TemporaryFile::deleteTemporaryFile() const
{
    // Have a few attempts at deleting the file before giving up..
    for (int i = 5; --i >= 0;)
    {
        if (temporaryFile.isDirectory() ? temporaryFile.deleteRecursively() : temporaryFile.deleteFile())
            return true;

        Thread::sleep (50);
    }

    return false;
}