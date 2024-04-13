changedir(char *path)
{
#if defined(MSDOS)
    /* first deal with drive letter */

    if (isalpha((unsigned char)path[0]) && (path[1] == ':')) {
	int driveno = toupper((unsigned char)path[0]) - 'A';	/* 0=A, 1=B, ... */

# if defined(__EMX__) || defined(__WATCOMC__)
	(void) _chdrive(driveno + 1);
# elif defined(__DJGPP__)
	(void) setdisk(driveno);
# endif
	path += 2;		/* move past drive letter */
    }
    /* then change to actual directory */
    if (*path)
	if (chdir(path))
	    return 1;

    return 0;			/* should report error with setdrive also */

#elif defined(_WIN32)
    LPWSTR pathw = UnicodeText(path, encoding);
    int ret = !SetCurrentDirectoryW(pathw);
    free(pathw);
    return ret;
#elif defined(__EMX__) && defined(OS2)
    return _chdir2(path);
#else
    return chdir(path);
#endif /* MSDOS etc. */
}