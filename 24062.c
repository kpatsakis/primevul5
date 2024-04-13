SCM_DEFINE (scm_open_fdes, "open-fdes", 2, 1, 0, 
            (SCM path, SCM flags, SCM mode),
	    "Similar to @code{open} but return a file descriptor instead of\n"
	    "a port.")
#define FUNC_NAME s_scm_open_fdes
{
  int fd;
  int iflags;
  int imode;

  iflags = SCM_NUM2INT (2, flags);
  imode = SCM_NUM2INT_DEF (3, mode, 0666);
  STRING_SYSCALL (path, c_path, fd = open_or_open64 (c_path, iflags, imode));
  if (fd == -1)
    SCM_SYSERROR;
  return scm_from_int (fd);
}