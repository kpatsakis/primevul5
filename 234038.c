change_dir ()
{
  if (change_directory_option && chdir (change_directory_option))
    {
      if (errno == ENOENT && create_dir_flag)
	{
	  if (make_path (change_directory_option, -1, -1,
			 (warn_option & CPIO_WARN_INTERDIR) ?
			 _("Creating directory `%s'") : NULL))
	    exit (PAXEXIT_FAILURE);

	  if (chdir (change_directory_option) == 0)
	    return;
	}
      error (PAXEXIT_FAILURE, errno,
	     _("cannot change to directory `%s'"), change_directory_option);
    }
}