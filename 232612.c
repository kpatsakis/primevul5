delete_recursive(char_u *name)
{
    int result = 0;
    char_u	**files;
    int		file_count;
    int		i;
    char_u	*exp;

    /* A symbolic link to a directory itself is deleted, not the directory it
     * points to. */
    if (
# if defined(UNIX) || defined(WIN32)
	 mch_isrealdir(name)
# else
	 mch_isdir(name)
# endif
	    )
    {
	vim_snprintf((char *)NameBuff, MAXPATHL, "%s/*", name);
	exp = vim_strsave(NameBuff);
	if (exp == NULL)
	    return -1;
	if (gen_expand_wildcards(1, &exp, &file_count, &files,
	      EW_DIR|EW_FILE|EW_SILENT|EW_ALLLINKS|EW_DODOT|EW_EMPTYOK) == OK)
	{
	    for (i = 0; i < file_count; ++i)
		if (delete_recursive(files[i]) != 0)
		    result = -1;
	    FreeWild(file_count, files);
	}
	else
	    result = -1;
	vim_free(exp);
	(void)mch_rmdir(name);
    }
    else
	result = mch_remove(name) == 0 ? 0 : -1;

    return result;
}