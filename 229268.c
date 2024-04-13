mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
		   const char *passwd, const char *db,
		   uint port, const char *unix_socket,unsigned long client_flag)
{
  char *end= NULL;
  char *connection_handler= (mysql->options.extension) ?
                            mysql->options.extension->connection_handler : 0;

  if (!mysql->methods)
    mysql->methods= &MARIADB_DEFAULT_METHODS;

  if (connection_handler ||
      (host && (end= strstr(host, "://"))))
  {
    MARIADB_CONNECTION_PLUGIN *plugin;
    char plugin_name[64];

    if (!connection_handler || !connection_handler[0])
    {
      memset(plugin_name, 0, 64);
      ma_strmake(plugin_name, host, MIN(end - host, 63));
      end+= 3;
    }
    else
      ma_strmake(plugin_name, connection_handler, MIN(63, strlen(connection_handler)));

    if (!(plugin= (MARIADB_CONNECTION_PLUGIN *)mysql_client_find_plugin(mysql, plugin_name, MARIADB_CLIENT_CONNECTION_PLUGIN)))
      return NULL;

    if (!(mysql->extension->conn_hdlr= (MA_CONNECTION_HANDLER *)calloc(1, sizeof(MA_CONNECTION_HANDLER))))
    {
      SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
      return NULL;
    }

    /* save URL for reconnect */
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, url, host);

    mysql->extension->conn_hdlr->plugin= plugin;

    if (plugin && plugin->connect)
    {
      MYSQL *my= plugin->connect(mysql, end, user, passwd, db, port, unix_socket, client_flag);
      if (!my)
      {
        free(mysql->extension->conn_hdlr);
        mysql->extension->conn_hdlr= NULL;
      }
      return my;
    }
  }
#ifndef HAVE_SCHANNEL
  return mysql->methods->db_connect(mysql, host, user, passwd,
                                    db, port, unix_socket, client_flag);
#else
/* 
   With older windows versions (prior Win 10) TLS connections periodically
   fail with SEC_E_INVALID_TOKEN, SEC_E_BUFFER_TOO_SMALL or SEC_E_MESSAGE_ALTERED
   error (see MDEV-13492). If the connect attempt returns on of these error codes
   in mysql->net.extended_errno we will try to connect again (max. 3 times)
*/
#define MAX_SCHANNEL_CONNECT_ATTEMPTS 3
  {
    int ssl_retry= (mysql->options.use_ssl) ? MAX_SCHANNEL_CONNECT_ATTEMPTS : 1;
	MYSQL *my= NULL;
    while (ssl_retry)
    {
      if ((my= mysql->methods->db_connect(mysql, host, user, passwd,
                                    db, port, unix_socket, client_flag | CLIENT_REMEMBER_OPTIONS)))
        return my;

      switch (mysql->net.extension->extended_errno) {
        case SEC_E_INVALID_TOKEN:
        case SEC_E_BUFFER_TOO_SMALL:
        case SEC_E_MESSAGE_ALTERED:
          ssl_retry--;
          break;
        default:
          ssl_retry= 0;
          break;
      }
    }
    if (!my && !(client_flag & CLIENT_REMEMBER_OPTIONS))
      mysql_close_options(mysql);
    return my;
  }
#endif
}