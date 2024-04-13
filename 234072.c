delay_cpio_set_stat (struct cpio_file_stat *file_stat,
		     mode_t invert_permissions)
{
  size_t file_name_len = strlen (file_stat->c_name);
  struct delayed_set_stat *data =
    xmalloc (sizeof (struct delayed_set_stat) + file_name_len + 1);
  data->next = delayed_set_stat_head;
  memcpy (&data->stat, file_stat, sizeof data->stat);
  data->stat.c_name = (char*) (data + 1);
  strcpy (data->stat.c_name, file_stat->c_name);
  data->invert_permissions = invert_permissions;
  delayed_set_stat_head = data;
}