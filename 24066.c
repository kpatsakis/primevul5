SCM_DEFINE (scm_dirname, "dirname", 1, 0, 0, 
            (SCM filename),
	    "Return the directory name component of the file name\n"
	    "@var{filename}. If @var{filename} does not contain a directory\n"
	    "component, @code{.} is returned.")
#define FUNC_NAME s_scm_dirname
{
  char *c_filename, *c_dirname;
  SCM res;

  scm_dynwind_begin (0);
  c_filename = scm_to_utf8_string (filename);
  scm_dynwind_free (c_filename);

  c_dirname = mdir_name (c_filename);
  if (!c_dirname)
    SCM_SYSERROR;
  scm_dynwind_free (c_dirname);

  res = scm_from_utf8_string (c_dirname);
  scm_dynwind_end ();

  return res;
}