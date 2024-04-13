    size_t writeFile(const DataBuf& buf, const std::wstring& wpath)
    {
        FileIo file(wpath);
        if (file.open("wb") != 0) {
            throw WError(kerFileOpenFailed, wpath, "wb", strError().c_str());
        }
        return file.write(buf.pData_, buf.size_);
    }