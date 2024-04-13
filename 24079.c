SCM_DEFINE (scm_readdir, "readdir", 1, 0, 0,
	    (SCM port),
	    "Return (as a string) the next directory entry from the directory stream\n"
	    "@var{port}.  If there is no remaining entry to be read then the\n"
	    "end of file object is returned.")
#define FUNC_NAME s_scm_readdir
{
  struct dirent_or_dirent64 *rdent;

  SCM_VALIDATE_DIR (1, port);
  if (!SCM_DIR_OPEN_P (port))
    SCM_MISC_ERROR ("Directory ~S is not open.", scm_list_1 (port));

#if HAVE_READDIR_R
  /* As noted in the glibc manual, on various systems (such as Solaris) the
     d_name[] field is only 1 char and you're expected to size the dirent
     buffer for readdir_r based on NAME_MAX.  The SCM_MAX expressions below
     effectively give either sizeof(d_name) or NAME_MAX+1, whichever is
     bigger.

     On solaris 10 there's no NAME_MAX constant, it's necessary to use
     pathconf().  We prefer NAME_MAX though, since it should be a constant
     and will therefore save a system call.  We also prefer it since dirfd()
     is not available everywhere.

     An alternative to dirfd() would be to open() the directory and then use
     fdopendir(), if the latter is available.  That'd let us hold the fd
     somewhere in the smob, or just the dirent size calculated once.  */
  {
    struct dirent_or_dirent64 de; /* just for sizeof */
    DIR    *ds = (DIR *) SCM_SMOB_DATA_1 (port);
#ifdef NAME_MAX
    char   buf [SCM_MAX (sizeof (de),
			 sizeof (de) - sizeof (de.d_name) + NAME_MAX + 1)];
#else
    char   *buf;
    long   name_max = fpathconf (dirfd (ds), _PC_NAME_MAX);
    if (name_max == -1)
      SCM_SYSERROR;
    buf = alloca (SCM_MAX (sizeof (de),
			   sizeof (de) - sizeof (de.d_name) + name_max + 1));
#endif

    errno = 0;
    SCM_SYSCALL (readdir_r_or_readdir64_r (ds, (struct dirent_or_dirent64 *) buf, &rdent));
    if (errno != 0)
      SCM_SYSERROR;
    if (! rdent)
      return SCM_EOF_VAL;

    return (rdent ? scm_from_locale_stringn (rdent->d_name, NAMLEN (rdent))
	    : SCM_EOF_VAL);
  }
#else
  {
    SCM ret;
    scm_dynwind_begin (0);
    scm_i_dynwind_pthread_mutex_lock (&scm_i_misc_mutex);

    errno = 0;
    SCM_SYSCALL (rdent = readdir_or_readdir64 ((DIR *) SCM_SMOB_DATA_1 (port)));
    if (errno != 0)
      SCM_SYSERROR;

    ret = (rdent ? scm_from_locale_stringn (rdent->d_name, NAMLEN (rdent))
	   : SCM_EOF_VAL);

    scm_dynwind_end ();
    return ret;
  }
#endif
}