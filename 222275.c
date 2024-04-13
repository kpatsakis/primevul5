lept_size_proc(thandle_t  cookie)
{
    FILE* fp = (FILE *)cookie;
#if defined(_MSC_VER)
    __int64 pos;
    __int64 size;
    if (!cookie || !fp)
        return (tsize_t)-1;
    pos = _ftelli64(fp);
    _fseeki64(fp, 0, SEEK_END);
    size = _ftelli64(fp);
    _fseeki64(fp, pos, SEEK_SET);
#elif defined(_LARGEFILE64_SOURCE)
    off64_t pos;
    off64_t size;
    if (!fp)
        return (tsize_t)-1;
    pos = ftello(fp);
    fseeko(fp, 0, SEEK_END);
    size = ftello(fp);
    fseeko(fp, pos, SEEK_SET);
#else
    off_t pos;
    off_t size;
    if (!cookie || !fp)
        return (tsize_t)-1;
    pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, pos, SEEK_SET);
#endif
    return (toff_t)size;
}