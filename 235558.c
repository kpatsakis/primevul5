std::string PasswdMgr::getPasswdByUserName(const std::string& userName)
{
    checkAndReload();
    auto iter = passwdMapList.find(userName);
    if (iter == passwdMapList.end())
    {
        return std::string();
    }
    return iter->second;
}