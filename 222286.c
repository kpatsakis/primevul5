lept_write_proc(thandle_t  cookie,
                tdata_t    buff,
                tsize_t    size)
{
    FILE* fp = (FILE *)cookie;
    tsize_t done;
    if (!buff || !cookie || !fp)
        return (tsize_t)-1;
    done = fwrite(buff, 1, size, fp);
    return done;
}