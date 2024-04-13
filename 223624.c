    DataBuf readFile(const std::wstring& wpath)
    {
        FileIo file(wpath);
        if (file.open("rb") != 0) {
            throw WError(kerFileOpenFailed, wpath, "rb", strError().c_str());
        }
        struct _stat st;
        if (0 != ::_wstat(wpath.c_str(), &st)) {
            throw WError(kerCallFailed, wpath, strError().c_str(), "::_wstat");
        }
        DataBuf buf(st.st_size);
        size_t len = file.read(buf.pData_, buf.size_);
        if (len != buf.size_) {
            throw WError(kerCallFailed, wpath, strError().c_str(), "FileIo::read");
        }
        return buf;
    }