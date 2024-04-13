is_file_name_separator (SCM c)
{
  if (scm_is_eq (c, SCM_MAKE_CHAR ('/')))
    return 1;
#ifdef __MINGW32__
  if (scm_is_eq (c, SCM_MAKE_CHAR ('\\')))
    return 1;
#endif
  return 0;
}