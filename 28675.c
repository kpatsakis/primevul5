dm_export (int major, int minor)
{
  gboolean ret;
  struct dm_task *dmt;
  void *next;
  uint64_t start, length;
  char *target_type;
  char *params;
  const char *name;
  struct dm_info info;
  GString *target_types_str;
  GString *start_str;
  GString *length_str;
  GString *params_str;
  gchar buf[4096];

  ret = FALSE;
  dmt = NULL;

  dmt = dm_task_create (DM_DEVICE_TABLE);
  if (dmt == NULL)
    {
      perror ("dm_task_create");
      goto out;
    }

  if (dm_task_set_major (dmt, major) == 0)
    {
      perror ("dm_task_set_major");
      goto out;
    }

  if (dm_task_set_minor (dmt, minor) == 0)
    {
      perror ("dm_task_set_minor");
      goto out;
    }

  if (dm_task_run (dmt) == 0)
    {
      perror ("dm_task_run");
      goto out;
    }

  if (dm_task_get_info (dmt, &info) == 0 || !info.exists)
    {
      perror ("dm_task_get_info");
      goto out;
    }

  name = dm_task_get_name (dmt);
  if (name == NULL)
    {
      perror ("dm_task_get_name");
      goto out;
    }

  if (!info.exists)
    {
      goto out;
    }

  if (info.target_count != -1)
    g_print ("UDISKS_DM_TARGETS_COUNT=%d\n", info.target_count);

  target_types_str = g_string_new (NULL);
  start_str = g_string_new (NULL);
  length_str = g_string_new (NULL);
  params_str = g_string_new (NULL);

  /* export all tables */
  next = NULL;
  do
    {
      next = dm_get_next_target (dmt, next, &start, &length, &target_type, &params);
      if (target_type != NULL)
        {
          g_string_append (target_types_str, target_type);
          g_string_append_printf (start_str, "%" G_GUINT64_FORMAT, start);
          g_string_append_printf (length_str, "%" G_GUINT64_FORMAT, length);
          if (params != NULL && strlen (params) > 0)
            {
              _udev_util_encode_string (params, buf, sizeof (buf));
              g_string_append (params_str, buf);
            }
        }

      if (next != NULL)
        {
          g_string_append_c (target_types_str, ' ');
          g_string_append_c (start_str, ' ');
          g_string_append_c (length_str, ' ');
          g_string_append_c (params_str, ' ');
        }
    }
  while (next != NULL);

  if (target_types_str->len > 0)
      g_print ("UDISKS_DM_TARGETS_TYPE=%s\n", target_types_str->str);
  if (start_str->len > 0)
      g_print ("UDISKS_DM_TARGETS_START=%s\n", start_str->str);
  if (length_str->len > 0)
      g_print ("UDISKS_DM_TARGETS_LENGTH=%s\n", length_str->str);
  if (params_str->len > 0)
      g_print ("UDISKS_DM_TARGETS_PARAMS=%s\n", params_str->str);

  g_string_free (target_types_str, TRUE);
  g_string_free (start_str, TRUE);
  g_string_free (length_str, TRUE);
  g_string_free (params_str, TRUE);

  ret = TRUE;

 out:
  if (dmt != NULL)
    dm_task_destroy(dmt);
  return ret;
}