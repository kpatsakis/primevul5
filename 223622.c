    int FileIo::Impl::stat(StructStat& buf) const
    {
        int ret = 0;
#ifdef EXV_UNICODE_PATH
#ifdef _WIN64
            struct _stat64 st;
            ret = ::_wstati64(wpath_.c_str(), &st);

            if (0 == ret) {
                buf.st_size = static_cast<off_t>(st.st_size);
                buf.st_mode = st.st_mode;
                buf.st_nlink = st.st_nlink;
            }
#else
            struct _stat st;
            ret = ::_wstat(wpath_.c_str(), &st);

            if (0 == ret) {
                buf.st_size = st.st_size;
                buf.st_mode = st.st_mode;
                buf.st_nlink = st.st_nlink;
            }
#endif
        else
#endif
        {
            struct stat st;
            ret = ::stat(path_.c_str(), &st);
            if (0 == ret) {
                buf.st_size = st.st_size;
                buf.st_nlink = st.st_nlink;
                buf.st_mode = st.st_mode;
            }
        }
        return ret;
    } // FileIo::Impl::stat