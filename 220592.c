open_exfile(
    char_u	*fname,
    int		forceit,
    char	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    if (mch_isdir(fname))
    {
	semsg(_(e_src_is_directory), fname);
	return NULL;
    }
#endif
    if (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_("E189: \"%s\" exists (add ! to override)"), fname);
	return NULL;
    }

    if ((fd = mch_fopen((char *)fname, mode)) == NULL)
	semsg(_("E190: Cannot open \"%s\" for writing"), fname);

    return fd;
}