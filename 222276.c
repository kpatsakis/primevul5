lept_close_proc(thandle_t  cookie)
{
    FILE* fp = (FILE *)cookie;
    if (!cookie || !fp)
        return 0;
    fseek(fp, 0, SEEK_SET);
    return 0;
}