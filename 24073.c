SCM_DEFINE (scm_rename, "rename-file", 2, 0, 0,
            (SCM oldname, SCM newname),
	    "Renames the file specified by @var{oldname} to @var{newname}.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_rename
{
  int rv;

  STRING2_SYSCALL (oldname, c_oldname,
		   newname, c_newname,
		   rv = rename (c_oldname, c_newname));
  if (rv != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}