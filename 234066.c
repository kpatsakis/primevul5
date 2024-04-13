repair_inter_delayed_set_stat (struct stat *dir_stat_info)
{
  struct delayed_set_stat *data;
  for (data = delayed_set_stat_head; data; data = data->next)
    {
      struct stat st;
      if (stat (data->stat.c_name, &st) != 0)
	{
	  stat_error (data->stat.c_name);
	  return -1;
	}

      if (st.st_dev == dir_stat_info->st_dev
	  && st.st_ino == dir_stat_info->st_ino)
	{
	  stat_to_cpio (&data->stat, dir_stat_info);
	  data->invert_permissions =
	    ((dir_stat_info->st_mode ^ st.st_mode)
	     & MODE_RWX & ~ newdir_umask);
	  return 0;
	}
    }
  return 1;
}