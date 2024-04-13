apply_delayed_set_stat ()
{
  while (delayed_set_stat_head)
    {
      struct delayed_set_stat *data = delayed_set_stat_head;
      if (data->invert_permissions)
	{
	  data->stat.c_mode ^= data->invert_permissions;
	}
      set_perms (-1, &data->stat);
      delayed_set_stat_head = data->next;
      free (data);
    }
}