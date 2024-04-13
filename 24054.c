SCM_DEFINE (scm_symlink, "symlink", 2, 0, 0,
            (SCM oldpath, SCM newpath),
	    "Create a symbolic link named @var{newpath} with the value\n"
	    "(i.e., pointing to) @var{oldpath}.  The return value is\n"
	    "unspecified.")
#define FUNC_NAME s_scm_symlink
{
  int val;

  STRING2_SYSCALL (oldpath, c_oldpath,
		   newpath, c_newpath,
		   val = symlink (c_oldpath, c_newpath));
  if (val != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}