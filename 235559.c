void PasswdMgr::checkAndReload(void)
{
    std::time_t updatedTime = getUpdatedFileTime();
    if (fileLastUpdatedTime != updatedTime && updatedTime != -1)
    {
        log<level::DEBUG>("Reloading password map list");
        passwdMapList.clear();
        initPasswordMap();
    }
}