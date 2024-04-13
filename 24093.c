SCM_DEFINE (scm_rmdir, "rmdir", 1, 0, 0, 
            (SCM path),
	    "Remove the existing directory named by @var{path}.  The directory must\n"
	    "be empty for this to succeed.  The return value is unspecified.")
#define FUNC_NAME s_scm_rmdir
{
  int val;

  STRING_SYSCALL (path, c_path, val = rmdir (c_path));
  if (val != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}