repair_delayed_set_stat (struct cpio_file_stat *file_hdr)
{
  struct delayed_set_stat *data;
  for (data = delayed_set_stat_head; data; data = data->next)
    {
      if (strcmp (file_hdr->c_name, data->stat.c_name) == 0)
	{
	  data->invert_permissions = 0;
	  memcpy (&data->stat, file_hdr,
		  offsetof (struct cpio_file_stat, c_name));
	  return 0;
	}
    }
  return 1;
}