create_all_directories (char const *name)
{
  char *dir;

  dir = dir_name (name);
  
  if (dir == NULL)
    error (PAXEXIT_FAILURE, 0, _("virtual memory exhausted"));

  if (dir[0] != '.' || dir[1] != '\0')
    {
      const char *fmt;
      if (warn_option & CPIO_WARN_INTERDIR)
	fmt = _("Creating intermediate directory `%s'");
      else
	fmt = NULL;
      make_path (dir, -1, -1, fmt);
    }

  free (dir);
}