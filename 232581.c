shorten_fname1(char_u *full_path)
{
    char_u	*dirname;
    char_u	*p = full_path;

    dirname = alloc(MAXPATHL);
    if (dirname == NULL)
	return full_path;
    if (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	if (p == NULL || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    return p;
}