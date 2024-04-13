SCM_DEFINE (scm_readlink, "readlink", 1, 0, 0, 
            (SCM path),
	    "Return the value of the symbolic link named by @var{path} (a\n"
	    "string), i.e., the file that the link points to.")
#define FUNC_NAME s_scm_readlink
{
  int rv;
  int size = 100;
  char *buf;
  SCM result;
  char *c_path;
  
  scm_dynwind_begin (0);

  c_path = scm_to_locale_string (path);
  scm_dynwind_free (c_path);

  buf = scm_malloc (size);

  while ((rv = readlink (c_path, buf, size)) == size)
    {
      free (buf);
      size *= 2;
      buf = scm_malloc (size);
    }
  if (rv == -1)
    {
      int save_errno = errno;
      free (buf);
      errno = save_errno;
      SCM_SYSERROR;
    }
  result = scm_take_locale_stringn (buf, rv);

  scm_dynwind_end ();
  return result;
}