lept_seek_proc(thandle_t  cookie,
               toff_t     offs,
               int        whence)
{
    FILE* fp = (FILE *)cookie;
#if defined(_MSC_VER)
    __int64 pos = 0;
    if (!cookie || !fp)
        return (tsize_t)-1;
    switch (whence) {
    case SEEK_SET:
        pos = 0;
        break;
    case SEEK_CUR:
        pos = ftell(fp);
        break;
    case SEEK_END:
        _fseeki64(fp, 0, SEEK_END);
        pos = _ftelli64(fp);
        break;
    }
    pos = (__int64)(pos + offs);
    _fseeki64(fp, pos, SEEK_SET);
    if (pos == _ftelli64(fp))
        return (tsize_t)pos;
#elif defined(_LARGEFILE64_SOURCE)
    off64_t pos = 0;
    if (!cookie || !fp)
        return (tsize_t)-1;
    switch (whence) {
    case SEEK_SET:
        pos = 0;
        break;
    case SEEK_CUR:
        pos = ftello(fp);
        break;
    case SEEK_END:
        fseeko(fp, 0, SEEK_END);
        pos = ftello(fp);
        break;
    }
    pos = (off64_t)(pos + offs);
    fseeko(fp, pos, SEEK_SET);
    if (pos == ftello(fp))
        return (tsize_t)pos;
#else
    off_t pos = 0;
    if (!cookie || !fp)
        return (tsize_t)-1;
    switch (whence) {
    case SEEK_SET:
        pos = 0;
        break;
    case SEEK_CUR:
        pos = ftell(fp);
        break;
    case SEEK_END:
        fseek(fp, 0, SEEK_END);
        pos = ftell(fp);
        break;
    }
    pos = (off_t)(pos + offs);
    fseek(fp, pos, SEEK_SET);
    if (pos == ftell(fp))
        return (tsize_t)pos;
#endif
    return (tsize_t)-1;
}