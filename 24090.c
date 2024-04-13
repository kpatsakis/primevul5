SCM_DEFINE (scm_chmod, "chmod", 2, 0, 0,
            (SCM object, SCM mode),
	    "Changes the permissions of the file referred to by\n"
	    "@var{object}.  @var{object} can be a string containing a file\n"
	    "name or a port or integer file descriptor which is open on a\n"
	    "file (in which case @code{fchmod} is used as the underlying\n"
	    "system call).  @var{mode} specifies the new permissions as a\n"
	    "decimal number, e.g., @code{(chmod \"foo\" #o755)}.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_chmod
{
  int rv;

  object = SCM_COERCE_OUTPORT (object);

#if HAVE_FCHMOD
  if (scm_is_integer (object) || SCM_OPFPORTP (object))
    {
      int fdes;
      if (scm_is_integer (object))
	fdes = scm_to_int (object);
      else
	fdes = SCM_FPORT_FDES (object);
      SCM_SYSCALL (rv = fchmod (fdes, scm_to_int (mode)));
    }
  else
#endif
    {
      STRING_SYSCALL (object, c_object,
		      rv = chmod (c_object, scm_to_int (mode)));
    }
  if (rv == -1)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}