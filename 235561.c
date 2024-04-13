void PasswdMgr::initPasswordMap(void)
{
    phosphor::user::shadow::Lock lock();
    std::vector<uint8_t> dataBuf;

    if (readPasswdFileData(dataBuf) != 0)
    {
        log<level::DEBUG>("Error in reading the encrypted pass file");
        return;
    }

    if (dataBuf.size() != 0)
    {
        // populate the user list with password
        char* outPtr = reinterpret_cast<char*>(dataBuf.data());
        char* nToken = NULL;
        char* linePtr = strtok_r(outPtr, "\n", &nToken);
        size_t lineSize = 0;
        while (linePtr != NULL)
        {
            size_t userEPos = 0;
            std::string lineStr(linePtr);
            if ((userEPos = lineStr.find(":")) != std::string::npos)
            {
                lineSize = lineStr.size();
                passwdMapList.emplace(
                    lineStr.substr(0, userEPos),
                    lineStr.substr(userEPos + 1, lineSize - (userEPos + 1)));
            }
            linePtr = strtok_r(NULL, "\n", &nToken);
        }
    }

    // Update the timestamp
    fileLastUpdatedTime = getUpdatedFileTime();
    return;
}