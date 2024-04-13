check_file_readonly(
    char_u	*fname,		/* full path to file */
    int		perm)		/* known permissions on file */
{
#ifndef USE_MCH_ACCESS
    int	    fd = 0;
#endif

    return (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((char *)fname, W_OK)
#else
	(fd = mch_open((char *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
	);
}