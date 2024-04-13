SCM_DEFINE (scm_delete_file, "delete-file", 1, 0, 0, 
           (SCM str),
	    "Deletes (or \"unlinks\") the file specified by @var{str}.")
#define FUNC_NAME s_scm_delete_file
{
  int ans;
  STRING_SYSCALL (str, c_str, ans = unlink (c_str));
  if (ans != 0)
    SCM_SYSERROR;
  return SCM_UNSPECIFIED;
}