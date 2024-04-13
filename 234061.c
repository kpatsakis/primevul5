cpio_safer_name_suffix (char *name, bool link_target, bool absolute_names,
			bool strip_leading_dots)
{
  char *p = safer_name_suffix (name, link_target, absolute_names);
  if (strip_leading_dots && strcmp (p, "./"))
    /* strip leading `./' from the filename.  */
    while (*p == '.' && *(p + 1) == '/')
      {
	++p;
	while (*p == '/')
	  ++p;
      }
  if (p != name)
    /* The 'p' string is shortened version of 'name' with one exception;  when
       the 'name' points to an empty string (buffer where name[0] == '\0') the
       'p' then points to static string ".".  So caller needs to ensure there
       are at least two bytes available in 'name' buffer so memmove succeeds. */
    memmove (name, p, (size_t)(strlen (p) + 1));
}