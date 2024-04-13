    std::string XPathIo::writeDataToFile(const std::string& orgPath) {
        Protocol prot = fileProtocol(orgPath);

        // generating the name for temp file.
        std::time_t timestamp = std::time(nullptr);
        std::stringstream ss;
        ss << timestamp << XPathIo::TEMP_FILE_EXT;
        std::string path = ss.str();

        if (prot == pStdin) {
            if (isatty(fileno(stdin)))
                throw Error(kerInputDataReadFailed);
#if defined(_MSC_VER) || defined(__MINGW__)
            // convert stdin to binary
            if (_setmode(_fileno(stdin), _O_BINARY) == -1)
                throw Error(kerInputDataReadFailed);
#endif
            std::ofstream fs(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
            // read stdin and write to the temp file.
            char readBuf[100*1024];
            std::streamsize readBufSize = 0;
            do {
                std::cin.read(readBuf, sizeof(readBuf));
                readBufSize = std::cin.gcount();
                if (readBufSize > 0) {
                    fs.write (readBuf, readBufSize);
                }
            } while(readBufSize);
            fs.close();
        } else if (prot == pDataUri) {
            std::ofstream fs(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
            // read data uri and write to the temp file.
            size_t base64Pos = orgPath.find("base64,");
            if (base64Pos == std::string::npos) {
                fs.close();
                throw Error(kerErrorMessage, "No base64 data");
            }

            std::string data = orgPath.substr(base64Pos+7);
            char* decodeData = new char[data.length()];
            long size = base64decode(data.c_str(), decodeData, data.length());
            if (size > 0) {
                fs.write(decodeData, size);
                fs.close();
            } else {
                fs.close();
                throw Error(kerErrorMessage, "Unable to decode base 64.");
            }
            delete[] decodeData;
        }

        return path;
    }