cpio_create_dir (struct cpio_file_stat *file_hdr, int existing_dir)
{
  int res;			/* Result of various function calls.  */
  int setstat_delayed = 0;
  
  if (to_stdout_option)
    return 0;
  
  /* Strip any trailing `/'s off the filename; tar puts
     them on.  We might as well do it here in case anybody
     else does too, since they cause strange things to happen.  */
  strip_trailing_slashes (file_hdr->c_name);

  /* Ignore the current directory.  It must already exist,
     and we don't want to change its permission, ownership
     or time.  */
  if (file_hdr->c_name[0] == '.' && file_hdr->c_name[1] == '\0')
    {
      return 0;
    }

  if (!existing_dir)
    res = cpio_mkdir (file_hdr, &setstat_delayed);
  else
    res = 0;
  if (res < 0 && create_dir_flag)
    {
      create_all_directories (file_hdr->c_name);
      res = cpio_mkdir (file_hdr, &setstat_delayed);
    }
  if (res < 0)
    {
      /* In some odd cases where the file_hdr->c_name includes `.',
	 the directory may have actually been created by
	 create_all_directories(), so the mkdir will fail
	 because the directory exists.  If that's the case,
	 don't complain about it.  */
      struct stat file_stat;
      if (errno != EEXIST)
	{
	  mkdir_error (file_hdr->c_name);
	  return -1;
	}
      if (lstat (file_hdr->c_name, &file_stat))
	{
	  stat_error (file_hdr->c_name);
	  return -1;
	}
      if (!(S_ISDIR (file_stat.st_mode)))
	{
	  error (0, 0, _("%s is not a directory"),
		 quotearg_colon (file_hdr->c_name));
	  return -1;
	}
    }

  if (!setstat_delayed && repair_delayed_set_stat (file_hdr))
    set_perms (-1, file_hdr);
  return 0;
}