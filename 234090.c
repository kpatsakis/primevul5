try_existing_file (struct cpio_file_stat* file_hdr, int in_file_des,
		   bool *existing_dir)
{
  struct stat file_stat;

  *existing_dir = false;
  if (lstat (file_hdr->c_name, &file_stat) == 0)
    {
      if (S_ISDIR (file_stat.st_mode)
	  && ((file_hdr->c_mode & CP_IFMT) == CP_IFDIR))
	{
	  /* If there is already a directory there that
	     we are trying to create, don't complain about
	     it.  */
	  *existing_dir = true;
	  return 0;
	}
      else if (!unconditional_flag
	       && file_hdr->c_mtime <= file_stat.st_mtime)
	{
	  error (0, 0, _("%s not created: newer or same age version exists"),
		 file_hdr->c_name);
	  tape_toss_input (in_file_des, file_hdr->c_filesize);
	  tape_skip_padding (in_file_des, file_hdr->c_filesize);
	  return -1;	/* Go to the next file.  */
	}
      else if (S_ISDIR (file_stat.st_mode) 
		? rmdir (file_hdr->c_name)
		: unlink (file_hdr->c_name))
	{
	  error (0, errno, _("cannot remove current %s"),
		 file_hdr->c_name);
	  tape_toss_input (in_file_des, file_hdr->c_filesize);
	  tape_skip_padding (in_file_des, file_hdr->c_filesize);
	  return -1;	/* Go to the next file.  */
	}
    }
  return 0;
}