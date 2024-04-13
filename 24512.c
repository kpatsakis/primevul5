bool maria_show_status(handlerton *hton,
                       THD *thd,
                       stat_print_fn *print,
                       enum ha_stat_type stat)
{
  const LEX_CSTRING *engine_name= hton_name(hton);
  switch (stat) {
  case HA_ENGINE_LOGS:
  {
    TRANSLOG_ADDRESS horizon= translog_get_horizon();
    uint32 last_file= LSN_FILE_NO(horizon);
    uint32 first_needed= translog_get_first_needed_file();
    uint32 first_file= translog_get_first_file(horizon);
    uint32 i;
    const char unknown[]= "unknown";
    const char needed[]= "in use";
    const char unneeded[]= "free";
    char path[FN_REFLEN];

    if (first_file == 0)
    {
      const char error[]= "error";
      print(thd, engine_name->str, engine_name->length,
            STRING_WITH_LEN(""), error, sizeof(error) - 1);
      break;
    }

    for (i= first_file; i <= last_file; i++)
    {
      char *file;
      const char *status;
      size_t length, status_len;
      MY_STAT stat_buff, *stat;
      const char error[]= "can't stat";
      char object[SHOW_MSG_LEN];
      file= translog_filename_by_fileno(i, path);
      if (!(stat= mysql_file_stat(key_file_translog, file, &stat_buff, MYF(0))))
      {
        status= error;
        status_len= sizeof(error) - 1;
        length= my_snprintf(object, SHOW_MSG_LEN, "Size unknown ; %s", file);
      }
      else
      {
        if (first_needed == 0)
        {
          status= unknown;
          status_len= sizeof(unknown) - 1;
        }
        else if (i < first_needed)
        {
          status= unneeded;
          status_len= sizeof(unneeded) - 1;
        }
        else
        {
          status= needed;
          status_len= sizeof(needed) - 1;
        }
        length= my_snprintf(object, SHOW_MSG_LEN, "Size %12llu ; %s",
                            (ulonglong) stat->st_size, file);
      }

      print(thd, engine_name->str, engine_name->length,
            object, length, status, status_len);
    }
    break;
  }
  case HA_ENGINE_STATUS:
  case HA_ENGINE_MUTEX:
  default:
    break;
  }
  return 0;
}