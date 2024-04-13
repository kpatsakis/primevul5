link_to_name (char const *link_name, char const *link_target)
{
  int res = link (link_target, link_name);
  if (res < 0 && create_dir_flag)
    {
      create_all_directories (link_name);
      res = link (link_target, link_name);
    }
  if (res == 0)
    {
      if (verbose_flag)
	error (0, 0, _("%s linked to %s"),
	       link_target, link_name);
    }
  else if (link_flag)
    {
      error (0, errno, _("cannot link %s to %s"),
	     link_target, link_name);
    }
  return res;
}