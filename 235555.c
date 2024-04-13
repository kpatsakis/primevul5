std::time_t PasswdMgr::getUpdatedFileTime()
{
    struct stat fileStat = {};
    if (stat(passwdFileName, &fileStat) != 0)
    {
        log<level::DEBUG>("Error - Getting passwd file time stamp");
        return -EIO;
    }
    return fileStat.st_mtime;
}