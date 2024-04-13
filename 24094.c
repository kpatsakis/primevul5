SCM_DEFINE (scm_basename, "basename", 1, 1, 0, 
            (SCM filename, SCM suffix),
	    "Return the base name of the file name @var{filename}. The\n"
	    "base name is the file name without any directory components.\n"
	    "If @var{suffix} is provided, and is equal to the end of\n"
	    "@var{filename}, it is removed also.")
#define FUNC_NAME s_scm_basename
{
  char *c_filename, *c_last_component;
  SCM res;

  scm_dynwind_begin (0);
  c_filename = scm_to_utf8_string (filename);
  scm_dynwind_free (c_filename);

  c_last_component = last_component (c_filename);
  if (!c_last_component)
    res = filename;
  else
    res = scm_from_utf8_string (c_last_component);
  scm_dynwind_end ();

  if (!SCM_UNBNDP (suffix) &&
      scm_is_true (scm_string_suffix_p (suffix, filename,
                                        SCM_UNDEFINED, SCM_UNDEFINED,
                                        SCM_UNDEFINED, SCM_UNDEFINED)))
    res = scm_c_substring
      (res, 0, scm_c_string_length (res) - scm_c_string_length (suffix));

  return res;
}