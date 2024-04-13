scm_i_relativize_path (SCM path, SCM in_path)
{
  char *str, *canon;
  SCM scanon;
  
  str = scm_to_locale_string (path);
  canon = canonicalize_file_name (str);
  free (str);
  
  if (!canon)
    return SCM_BOOL_F;

  scanon = scm_take_locale_string (canon);

  for (; scm_is_pair (in_path); in_path = scm_cdr (in_path))
    {
      SCM dir = scm_car (in_path);
      size_t len = scm_c_string_length (dir);

      /* When DIR is empty, it means "current working directory".  We
	 could set DIR to (getcwd) in that case, but then the
	 canonicalization would depend on the current directory, which
	 is not what we want in the context of `compile-file', for
	 instance.  */
      if (len > 0
	  && scm_is_true (scm_string_prefix_p (dir, scanon,
					       SCM_UNDEFINED, SCM_UNDEFINED,
					       SCM_UNDEFINED, SCM_UNDEFINED)))
	{
	  /* DIR either has a trailing delimiter or doesn't.  SCANON
	     will be delimited by single delimiters.  When DIR does not
	     have a trailing delimiter, add one to the length to strip
	     off the delimiter within SCANON.  */
	  if (!is_file_name_separator (scm_c_string_ref (dir, len - 1)))
	    len++;

	  if (scm_c_string_length (scanon) > len)
	    return scm_substring (scanon, scm_from_size_t (len), SCM_UNDEFINED);
	  else
	    return SCM_BOOL_F;
	}
    }

  return SCM_BOOL_F;
}