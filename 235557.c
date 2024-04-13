int PasswdMgr::updateUserEntry(const std::string& userName,
                               const std::string& newUserName)
{
    std::time_t updatedTime = getUpdatedFileTime();
    // Check file time stamp to know passwdMapList is up-to-date.
    // If not up-to-date, then updatePasswdSpecialFile will read and
    // check the user entry existance.
    if (fileLastUpdatedTime == updatedTime && updatedTime != -EIO)
    {
        if (passwdMapList.find(userName) == passwdMapList.end())
        {
            log<level::DEBUG>("User not found");
            return 0;
        }
    }

    // Write passwdMap to Encryted file
    if (updatePasswdSpecialFile(userName, newUserName) != 0)
    {
        log<level::DEBUG>("Passwd file update failed");
        return -EIO;
    }

    log<level::DEBUG>("Passwd file updated successfully");
    return 0;
}