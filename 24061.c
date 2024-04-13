SCM_DEFINE (scm_canonicalize_path, "canonicalize-path", 1, 0, 0, 
            (SCM path),
	    "Return the canonical path of @var{path}. A canonical path has\n"
            "no @code{.} or @code{..} components, nor any repeated path\n"
            "separators (@code{/}) nor symlinks.\n\n"
            "Raises an error if any component of @var{path} does not exist.")
#define FUNC_NAME s_scm_canonicalize_path
{
  char *str, *canon;
  
  SCM_VALIDATE_STRING (1, path);

  str = scm_to_locale_string (path);
  canon = canonicalize_file_name (str);
  free (str);
  
  if (canon)
    return scm_take_locale_string (canon);
  else
    SCM_SYSERROR;
}