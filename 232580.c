vim_settempdir(char_u *tempdir)
{
    char_u	*buf;

    buf = alloc((unsigned)MAXPATHL + 2);
    if (buf != NULL)
    {
	if (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	    STRCPY(buf, tempdir);
	add_pathsep(buf);
	vim_tempdir = vim_strsave(buf);
	vim_free(buf);
    }
}