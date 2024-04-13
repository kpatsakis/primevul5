buf_modname(
    int	    shortname,		/* use 8.3 file name */
    char_u  *fname,
    char_u  *ext,
    int	    prepend_dot)	/* may prepend a '.' to file name */
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    int		fnamelen, extlen;

    extlen = (int)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    if (fname == NULL || *fname == NUL)
    {
	retval = alloc((unsigned)(MAXPATHL + extlen + 3));
	if (retval == NULL)
	    return NULL;
	if (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (int)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    return NULL;
	}
	if (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    /* nothing to prepend a dot to */
    }
    else
    {
	fnamelen = (int)STRLEN(fname);
	retval = alloc((unsigned)(fnamelen + extlen + 3));
	if (retval == NULL)
	    return NULL;
	STRCPY(retval, fname);
#ifdef VMS
	vms_remove_version(retval); /* we do not need versions here */
#endif
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    for (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	if (*ext == '.'
#ifdef USE_LONG_FNAME
		    && (!USE_LONG_FNAME || shortname)
#else
		    && shortname
#endif
								)
	    if (*ptr == '.')	/* replace '.' by '_' */
		*ptr = '_';
	if (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    break;
	}
    }

    /* the file name has at most BASENAMELEN characters. */
    if (STRLEN(ptr) > (unsigned)BASENAMELEN)
	ptr[BASENAMELEN] = '\0';

    s = ptr + STRLEN(ptr);

    /*
     * For 8.3 file names we may have to reduce the length.
     */
#ifdef USE_LONG_FNAME
    if (!USE_LONG_FNAME || shortname)
#else
    if (shortname)
#endif
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	if (fname == NULL || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    if (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	else if (*ext == '.')
	{
	    if ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	else if ((e = vim_strchr(ptr, '.')) == NULL)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	else if ((int)STRLEN(e) + extlen > 4)
	    s = e + 4 - extlen;
    }
#if defined(USE_LONG_FNAME) || defined(WIN3264)
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    else if ((fname == NULL || *fname == NUL) && *ext == '.')
	*s++ = '_';
#endif

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);

    /*
     * Prepend the dot.
     */
    if (prepend_dot && !shortname && *(e = gettail(retval)) != '.'
#ifdef USE_LONG_FNAME
	    && USE_LONG_FNAME
#endif
				)
    {
	STRMOVE(e + 1, e);
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    if (fname != NULL && STRCMP(fname, retval) == 0)
    {
	/* we search for a character that can be replaced by '_' */
	while (--s >= ptr)
	{
	    if (*s != '_')
	    {
		*s = '_';
		break;
	    }
	}
	if (s < ptr)	/* fname was "________.<ext>", how tricky! */
	    *ptr = 'v';
    }
    return retval;
}