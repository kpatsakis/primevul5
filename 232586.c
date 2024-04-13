vim_deltempdir(void)
{
    if (vim_tempdir != NULL)
    {
	/* remove the trailing path separator */
	gettail(vim_tempdir)[-1] = NUL;
	delete_recursive(vim_tempdir);
	vim_free(vim_tempdir);
	vim_tempdir = NULL;
    }
}