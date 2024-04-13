mysql_init(MYSQL *mysql)
{
  if (mysql_server_init(0, NULL, NULL))
    return NULL;
  if (!mysql)
  {
    if (!(mysql=(MYSQL*) calloc(1, sizeof(MYSQL))))
      return 0;
    mysql->free_me=1;
    mysql->net.pvio= 0;
    mysql->net.extension= 0;
  }
  else
  {
    memset((char*) (mysql), 0, sizeof(*(mysql)));
    mysql->net.pvio= 0;
    mysql->free_me= 0;
    mysql->net.extension= 0;
  }

  if (!(mysql->net.extension= (struct st_mariadb_net_extension *)
                               calloc(1, sizeof(struct st_mariadb_net_extension))) ||
      !(mysql->extension= (struct st_mariadb_extension *)
                          calloc(1, sizeof(struct st_mariadb_extension))))
    goto error;
  mysql->options.report_data_truncation= 1;
  mysql->options.connect_timeout=CONNECT_TIMEOUT;
  mysql->charset= mysql_find_charset_name(MARIADB_DEFAULT_CHARSET);
  mysql->methods= &MARIADB_DEFAULT_METHODS;
  strcpy(mysql->net.sqlstate, "00000");
  mysql->net.last_error[0]= mysql->net.last_errno= mysql->net.extension->extended_errno= 0;

  if (ENABLED_LOCAL_INFILE != LOCAL_INFILE_MODE_OFF)
    mysql->options.client_flag|= CLIENT_LOCAL_FILES;
  mysql->extension->auto_local_infile= ENABLED_LOCAL_INFILE == LOCAL_INFILE_MODE_AUTO
                                       ? WAIT_FOR_QUERY : ALWAYS_ACCEPT;
  mysql->options.reconnect= 0;
  return mysql;
error:
  if (mysql->free_me)
    free(mysql);
  return 0;
}