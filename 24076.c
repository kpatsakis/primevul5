SCM_DEFINE (scm_lstat, "lstat", 1, 0, 0, 
            (SCM str),
	    "Similar to @code{stat}, but does not follow symbolic links, i.e.,\n"
	    "it will return information about a symbolic link itself, not the\n"
	    "file it points to.  @var{str} must be a string.")
#define FUNC_NAME s_scm_lstat
{
  int rv;
  struct stat_or_stat64 stat_temp;

  STRING_SYSCALL (str, c_str, rv = lstat_or_lstat64 (c_str, &stat_temp));
  if (rv != 0)
    {
      int en = errno;

      SCM_SYSERROR_MSG ("~A: ~S",
			scm_list_2 (scm_strerror (scm_from_int (en)), str),
			en);
    }
  return scm_stat2scm (&stat_temp);
}