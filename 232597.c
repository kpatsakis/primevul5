vim_tempname(
    int	    extra_char UNUSED,  /* char to use in the name instead of '?' */
    int	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	/* use tmpnam() */
#else
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    static char	*(tempdirs[]) = {TEMPDIRNAMES};
    int		i;
# ifndef EEXIST
    stat_T	st;
# endif

    /*
     * This will create a directory for private use by this instance of Vim.
     * This is done once, and the same directory is used for all temp files.
     * This method avoids security problems because of symlink attacks et al.
     * It's also a bit faster, because we only need to check for an existing
     * file when creating the directory and not for each temp file.
     */
    if (vim_tempdir == NULL)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	for (i = 0; i < (int)(sizeof(tempdirs) / sizeof(char *)); ++i)
	{
# ifndef HAVE_MKDTEMP
	    size_t	itmplen;
	    long	nr;
	    long	off;
# endif

	    /* Expand $TMP, leave room for "/v1100000/999999999".
	     * Skip the directory check if the expansion fails. */
	    expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    if (itmp[0] != '$' && mch_isdir(itmp))
	    {
		/* directory exists */
		add_pathsep(itmp);

# ifdef HAVE_MKDTEMP
		{
#  if defined(UNIX) || defined(VMS)
		    /* Make sure the umask doesn't remove the executable bit.
		     * "repl" has been reported to use "177". */
		    mode_t	umask_save = umask(077);
#  endif
		    /* Leave room for filename */
		    STRCAT(itmp, "vXXXXXX");
		    if (mkdtemp((char *)itmp) != NULL)
			vim_settempdir(itmp);
#  if defined(UNIX) || defined(VMS)
		    (void)umask(umask_save);
#  endif
		}
# else
		/* Get an arbitrary number of up to 6 digits.  When it's
		 * unlikely that it already exists it will be faster,
		 * otherwise it doesn't matter.  The use of mkdir() avoids any
		 * security problems because of the predictable number. */
		nr = (mch_get_pid() + (long)time(NULL)) % 1000000L;
		itmplen = STRLEN(itmp);

		/* Try up to 10000 different values until we find a name that
		 * doesn't exist. */
		for (off = 0; off < 10000L; ++off)
		{
		    int		r;
#  if defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    sprintf((char *)itmp + itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    /* If mkdir() does not set errno to EEXIST, check for
		     * existing file here.  There is a race condition then,
		     * although it's fail-safe. */
		    if (mch_stat((char *)itmp, &st) >= 0)
			continue;
#  endif
#  if defined(UNIX) || defined(VMS)
		    /* Make sure the umask doesn't remove the executable bit.
		     * "repl" has been reported to use "177". */
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  if defined(UNIX) || defined(VMS)
		    (void)umask(umask_save);
#  endif
		    if (r == 0)
		    {
			vim_settempdir(itmp);
			break;
		    }
#  ifdef EEXIST
		    /* If the mkdir() didn't fail because the file/dir exists,
		     * we probably can't create any dir here, try another
		     * place. */
		    if (errno != EEXIST)
#  endif
			break;
		}
# endif /* HAVE_MKDTEMP */
		if (vim_tempdir != NULL)
		    break;
	    }
	}
    }

    if (vim_tempdir != NULL)
    {
	/* There is no need to check if the file exists, because we own the
	 * directory and nobody else creates a file in it. */
	sprintf((char *)itmp, "%s%ld", vim_tempdir, temp_count++);
	return vim_strsave(itmp);
    }

    return NULL;

#else /* TEMPDIRNAMES */

# ifdef WIN3264
    char	szTempFile[_MAX_PATH + 1];
    char	buf4[4];
    char_u	*retval;
    char_u	*p;

    STRCPY(itmp, "");
    if (GetTempPath(_MAX_PATH, szTempFile) == 0)
    {
	szTempFile[0] = '.';	/* GetTempPath() failed, use current dir */
	szTempFile[1] = NUL;
    }
    strcpy(buf4, "VIM");
    buf4[2] = extra_char;   /* make it "VIa", "VIb", etc. */
    if (GetTempFileName(szTempFile, buf4, 0, (LPSTR)itmp) == 0)
	return NULL;
    if (!keep)
	/* GetTempFileName() will create the file, we don't want that */
	(void)DeleteFile((LPSTR)itmp);

    /* Backslashes in a temp file name cause problems when filtering with
     * "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
     * didn't set 'shellslash'. */
    retval = vim_strsave(itmp);
    if (*p_shcf == '-' || p_ssl)
	for (p = retval; *p; ++p)
	    if (*p == '\\')
		*p = '/';
    return retval;

# else /* WIN3264 */

#  ifdef USE_TMPNAM
    char_u	*p;

    /* tmpnam() will make its own name */
    p = tmpnam((char *)itmp);
    if (p == NULL || *p == NUL)
	return NULL;
#  else
    char_u	*p;

#   ifdef VMS_TEMPNAM
    /* mktemp() is not working on VMS.  It seems to be
     * a do-nothing function. Therefore we use tempnam().
     */
    sprintf((char *)itmp, "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (char *)itmp);
    if (p != NULL)
    {
	/* VMS will use '.LIS' if we don't explicitly specify an extension,
	 * and VIM will then be unable to find the file later */
	STRCPY(itmp, p);
	STRCAT(itmp, ".txt");
	free(p);
    }
    else
	return NULL;
#   else
    STRCPY(itmp, TEMPNAME);
    if ((p = vim_strchr(itmp, '?')) != NULL)
	*p = extra_char;
    if (mktemp((char *)itmp) == NULL)
	return NULL;
#   endif
#  endif

    return vim_strsave(itmp);
# endif /* WIN3264 */
#endif /* TEMPDIRNAMES */
}