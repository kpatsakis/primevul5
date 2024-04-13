lept_read_proc(thandle_t  cookie,
               tdata_t    buff,
               tsize_t    size)
{
    FILE* fp = (FILE *)cookie;
    tsize_t done;
    if (!buff || !cookie || !fp)
        return (tsize_t)-1;
    done = fread(buff, 1, size, fp);
    return done;
}