SCM_DEFINE (scm_chown, "chown", 3, 0, 0, 
            (SCM object, SCM owner, SCM group),
	    "Change the ownership and group of the file referred to by @var{object} to\n"
	    "the integer values @var{owner} and @var{group}.  @var{object} can be\n"
	    "a string containing a file name or, if the platform\n"
	    "supports fchown, a port or integer file descriptor\n"
	    "which is open on the file.  The return value\n"
	    "is unspecified.\n\n"
	    "If @var{object} is a symbolic link, either the\n"
	    "ownership of the link or the ownership of the referenced file will be\n"
	    "changed depending on the operating system (lchown is\n"
	    "unsupported at present).  If @var{owner} or @var{group} is specified\n"
	    "as @code{-1}, then that ID is not changed.")
#define FUNC_NAME s_scm_chown
{
  int rv;

  object = SCM_COERCE_OUTPORT (object);

#ifdef HAVE_FCHOWN
  if (scm_is_integer (object) || (SCM_OPFPORTP (object)))
    {
      int fdes = (SCM_OPFPORTP (object)?
		  SCM_FPORT_FDES (object) : scm_to_int (object));

      SCM_SYSCALL (rv = fchown (fdes, scm_to_int (owner), scm_to_int (group)));
    }
  else
#endif
    {
      STRING_SYSCALL (object, c_object,
		      rv = chown (c_object,
				  scm_to_int (owner), scm_to_int (group)));
    }
  if (rv == -1)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}