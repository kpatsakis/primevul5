set_file_time(
    char_u  *fname,
    time_t  atime,	    /* access time */
    time_t  mtime)	    /* modification time */
{
# if defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    struct utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (void)utime((char *)fname, &buf);
# else
#  if defined(HAVE_UTIMES)
    struct timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
#   ifdef NeXT
    (void)utimes((char *)fname, tvp);
#   else
    (void)utimes((char *)fname, (const struct timeval *)&tvp);
#   endif
#  endif
# endif
}