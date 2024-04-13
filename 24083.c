SCM_DEFINE (scm_chdir, "chdir", 1, 0, 0, 
            (SCM str),
	    "Change the current working directory to @var{str}.\n"
	    "The return value is unspecified.")
#define FUNC_NAME s_scm_chdir
{
  int ans;

  STRING_SYSCALL (str, c_str, ans = chdir (c_str));
  if (ans != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}