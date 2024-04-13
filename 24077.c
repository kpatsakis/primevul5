SCM_DEFINE (scm_getcwd, "getcwd", 0, 0, 0,
            (),
	    "Return the name of the current working directory.")
#define FUNC_NAME s_scm_getcwd
{
  char *rv;
  size_t size = 100;
  char *wd;
  SCM result;

  wd = scm_malloc (size);
  while ((rv = getcwd (wd, size)) == 0 && errno == ERANGE)
    {
      free (wd);
      size *= 2;
      wd = scm_malloc (size);
    }
  if (rv == 0)
    {
      int save_errno = errno;
      free (wd);
      errno = save_errno;
      SCM_SYSERROR;
    }
  /* On Windows, convert backslashes in current directory to forward
     slashes.  */
  scm_i_mirror_backslashes (wd);
  result = scm_from_locale_stringn (wd, strlen (wd));
  free (wd);
  return result;
}