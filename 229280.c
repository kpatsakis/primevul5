MYSQL *mthd_my_real_connect(MYSQL *mysql, const char *host, const char *user,
		   const char *passwd, const char *db,
		   uint port, const char *unix_socket, unsigned long client_flag)
{
  char		buff[NAME_LEN+USERNAME_LENGTH+100];
  char		*end, *end_pkt, *host_info;
  MA_PVIO_CINFO  cinfo= {NULL, NULL, 0, -1, NULL};
  MARIADB_PVIO   *pvio= NULL;
  char    *scramble_data;
  my_bool is_maria= 0;
  const char *scramble_plugin;
  uint pkt_length, scramble_len, pkt_scramble_len= 0;
  NET	*net= &mysql->net;

  if (!mysql->methods)
    mysql->methods= &MARIADB_DEFAULT_METHODS;

  if (net->pvio)  /* check if we are already connected */
  {
    SET_CLIENT_ERROR(mysql, CR_ALREADY_CONNECTED, SQLSTATE_UNKNOWN, 0);
    return(NULL);
  }

  /* use default options */
  if (mysql->options.my_cnf_file || mysql->options.my_cnf_group)
  {
    _mariadb_read_options(mysql, NULL,
			  (mysql->options.my_cnf_file ?
			   mysql->options.my_cnf_file : NULL),
			   mysql->options.my_cnf_group, 0);
    free(mysql->options.my_cnf_file);
    free(mysql->options.my_cnf_group);
    mysql->options.my_cnf_file=mysql->options.my_cnf_group=0;
  }

  if (!host || !host[0])
    host = mysql->options.host;

  ma_set_connect_attrs(mysql, host);

#ifndef WIN32
  if (mysql->options.protocol > MYSQL_PROTOCOL_SOCKET)
  {
    SET_CLIENT_ERROR(mysql, CR_CONN_UNKNOWN_PROTOCOL, SQLSTATE_UNKNOWN, 0);
    return(NULL);
  }
#endif

  /* Some empty-string-tests are done because of ODBC */
  if (!user || !user[0])
    user=mysql->options.user;
  if (!passwd)
  {
    passwd=mysql->options.password;
#ifndef DONT_USE_MYSQL_PWD
    if (!passwd)
      passwd=getenv("MYSQL_PWD");  /* get it from environment (haneke) */
    if (!passwd)
      passwd= "";
#endif
  }
  if (!db || !db[0])
    db=mysql->options.db;
  if (!port)
    port=mysql->options.port;
  if (!unix_socket)
    unix_socket=mysql->options.unix_socket;

  mysql->server_status=SERVER_STATUS_AUTOCOMMIT;

  /* try to connect via pvio_init */
  cinfo.host= host;
  cinfo.unix_socket= unix_socket;
  cinfo.port= port;
  cinfo.mysql= mysql;

  /*
  ** Grab a socket and connect it to the server
  */
#ifndef _WIN32
#if defined(HAVE_SYS_UN_H)
  if ((!host ||  strcmp(host,LOCAL_HOST) == 0) &&
      mysql->options.protocol != MYSQL_PROTOCOL_TCP &&
      (unix_socket || mysql_unix_port))
  {
    cinfo.host= LOCAL_HOST;
    cinfo.unix_socket= (unix_socket) ? unix_socket : mysql_unix_port;
    cinfo.type= PVIO_TYPE_UNIXSOCKET;
    sprintf(host_info=buff,ER(CR_LOCALHOST_CONNECTION),cinfo.host);
  }
  else
#endif
#else
  if (mysql->options.protocol == MYSQL_PROTOCOL_MEMORY)
  {
    cinfo.host= mysql->options.shared_memory_base_name;
    cinfo.type= PVIO_TYPE_SHAREDMEM;
    sprintf(host_info=buff,ER(CR_SHARED_MEMORY_CONNECTION), cinfo.host ? cinfo.host : SHM_DEFAULT_NAME);
  }
   /* named pipe */
  else if (mysql->options.protocol == MYSQL_PROTOCOL_PIPE ||
	  (host && strcmp(host,LOCAL_HOST_NAMEDPIPE) == 0))
  {
    cinfo.type= PVIO_TYPE_NAMEDPIPE;
    sprintf(host_info=buff,ER(CR_NAMEDPIPE_CONNECTION),cinfo.host);
  }
  else
#endif
  {
    cinfo.unix_socket=0;				/* This is not used */
    if (!port)
      port=mysql_port;
    if (!host)
      host=LOCAL_HOST;
    cinfo.host= host;
    cinfo.port= port;
    cinfo.type= PVIO_TYPE_SOCKET;
    sprintf(host_info=buff,ER(CR_TCP_CONNECTION), cinfo.host);
  }
  /* Initialize and load pvio plugin */
  if (!(pvio= ma_pvio_init(&cinfo)))
    goto error;

  /* try to connect */
  if (ma_pvio_connect(pvio, &cinfo) != 0)
  {
    ma_pvio_close(pvio);
    goto error;
  }

  if (mysql->options.extension && mysql->options.extension->proxy_header)
  {
    char *hdr = mysql->options.extension->proxy_header;
    size_t len = mysql->options.extension->proxy_header_len;
    if (ma_pvio_write(pvio, (unsigned char *)hdr, len) <= 0)
    {
      ma_pvio_close(pvio);
      goto error;
    }
  }

  if (ma_net_init(net, pvio))
    goto error;

  if (mysql->options.max_allowed_packet)
    net->max_packet_size= mysql->options.max_allowed_packet;

  ma_pvio_keepalive(net->pvio);
  strcpy(mysql->net.sqlstate, "00000");

  /* Get version info */
  mysql->protocol_version= PROTOCOL_VERSION;	/* Assume this */
/*
  if (ma_pvio_wait_io_or_timeout(net->pvio, FALSE, 0) < 1)
  {
    my_set_error(mysql, CR_SERVER_LOST, SQLSTATE_UNKNOWN,
                 ER(CR_SERVER_LOST_EXTENDED),
                 "handshake: waiting for initial communication packet",
                 errno);
    goto error;
  }
 */
  if ((pkt_length=ma_net_safe_read(mysql)) == packet_error)
  {
    if (mysql->net.last_errno == CR_SERVER_LOST)
      my_set_error(mysql, CR_SERVER_LOST, SQLSTATE_UNKNOWN,
                 ER(CR_SERVER_LOST_EXTENDED),
                 "handshake: reading initial communication packet",
                 errno);

    goto error;
  }
  end= (char *)net->read_pos;
  end_pkt= (char *)net->read_pos + pkt_length;

  /* Check if version of protocol matches current one */

  mysql->protocol_version= end[0];
  end++;

  /* Check if server sends an error */
  if (mysql->protocol_version == 0XFF)
  {
    net_get_error(end, pkt_length - 1, net->last_error, sizeof(net->last_error),
      &net->last_errno, net->sqlstate);
    /* fix for bug #26426 */
    if (net->last_errno == 1040)
      memcpy(net->sqlstate, "08004", SQLSTATE_LENGTH);
    goto error;
  }

  if (mysql->protocol_version <  PROTOCOL_VERSION)
  {
    net->last_errno= CR_VERSION_ERROR;
    sprintf(net->last_error, ER(CR_VERSION_ERROR), mysql->protocol_version,
	    PROTOCOL_VERSION);
    goto error;
  }
  /* Save connection information */
  if (!user) user="";

  if (!(mysql->host_info= strdup(host_info)) ||
      !(mysql->host= strdup(cinfo.host ? cinfo.host : "")) ||
      !(mysql->user=strdup(user)) ||
      !(mysql->passwd=strdup(passwd)))
  {
    SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
    goto error;
  }
  if (cinfo.unix_socket)
    mysql->unix_socket= strdup(cinfo.unix_socket);
  else
    mysql->unix_socket=0;
  mysql->port=port;
  client_flag|=mysql->options.client_flag;

  if (strncmp(end, MA_RPL_VERSION_HACK, sizeof(MA_RPL_VERSION_HACK) - 1) == 0)
  {
    mysql->server_version= strdup(end + sizeof(MA_RPL_VERSION_HACK) - 1);
    is_maria= 1;
  }
  else
  {
    if (!(mysql->server_version= strdup(end)))
    {
      SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
      goto error;
    }
  }
  end+= strlen(end) + 1;

  mysql->thread_id=uint4korr(end);
  end+=4;

  /* This is the first part of scramble packet. In 4.1 and later
     a second package will follow later */
  scramble_data= end;
  scramble_len= SCRAMBLE_LENGTH_323 + 1;
  scramble_plugin= old_password_plugin_name;
  end+= SCRAMBLE_LENGTH_323;

  /* 1st pad */
  end++;

  if (end + 1<= end_pkt)
  {
    mysql->server_capabilities=uint2korr(end);
  }

  /* mysql 5.5 protocol */
  if (end + 18 <= end_pkt)
  {
    mysql->server_language= uint1korr(end + 2);
    mysql->server_status= uint2korr(end + 3);
    mysql->server_capabilities|= (unsigned int)(uint2korr(end + 5)) << 16;
    pkt_scramble_len= uint1korr(end + 7);

    /* check if MariaD2B specific capabilities are available */
    if (is_maria && !(mysql->server_capabilities & CLIENT_MYSQL))
    {
      mysql->extension->mariadb_server_capabilities= (ulonglong) uint4korr(end + 14);
    }
  }

  /* pad 2 */
  end+= 18;

  /* second scramble package */
  if (end + SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323 + 1 <= end_pkt)
  {
    memcpy(end - SCRAMBLE_LENGTH_323, scramble_data, SCRAMBLE_LENGTH_323);
    scramble_data= end - SCRAMBLE_LENGTH_323;
    if (mysql->server_capabilities & CLIENT_PLUGIN_AUTH)
    {
      scramble_len= pkt_scramble_len;
      scramble_plugin= scramble_data + scramble_len;
      if (scramble_data + scramble_len > end_pkt)
      {
        SET_CLIENT_ERROR(mysql, CR_MALFORMED_PACKET, SQLSTATE_UNKNOWN, 0);
        goto error;
      }
    } else
    {
      scramble_len= (uint)(end_pkt - scramble_data);
      scramble_plugin= native_password_plugin_name;
    }
  } else
  {
    mysql->server_capabilities&= ~CLIENT_SECURE_CONNECTION;
    if (mysql->options.secure_auth)
    {
      SET_CLIENT_ERROR(mysql, CR_SECURE_AUTH, SQLSTATE_UNKNOWN, 0);
      goto error;
    }
  }

  /* Set character set */
  if (mysql->options.charset_name)
    mysql->charset= mysql_find_charset_name(mysql->options.charset_name);
  else
    mysql->charset=mysql_find_charset_name(MARIADB_DEFAULT_CHARSET);

  if (!mysql->charset)
  {
    net->last_errno=CR_CANT_READ_CHARSET;
    sprintf(net->last_error,ER(net->last_errno),
      mysql->options.charset_name ? mysql->options.charset_name : 
                                    MARIADB_DEFAULT_CHARSET,
      "compiled_in");
    goto error;
  }

  mysql->client_flag= client_flag;

  if (run_plugin_auth(mysql, scramble_data, scramble_len,
                             scramble_plugin, db))
    goto error;

  if (mysql->client_flag & CLIENT_COMPRESS)
    net->compress= 1;

  /* last part: select default db */
  if (!(mysql->server_capabilities & CLIENT_CONNECT_WITH_DB) &&
      (db && !mysql->db))
  {
    if (mysql_select_db(mysql, db))
    {
      my_set_error(mysql, CR_SERVER_LOST, SQLSTATE_UNKNOWN,
                          ER(CR_SERVER_LOST_EXTENDED),
                          "Setting intital database",
                          errno);
      goto error;
    }
  }

  if (mysql->options.init_command)
  {
    char **begin= (char **)mysql->options.init_command->buffer;
    char **end= begin + mysql->options.init_command->elements;

    /* Avoid reconnect in mysql_real_connect */
    my_bool save_reconnect= mysql->options.reconnect;
    mysql->options.reconnect= 0;

    for (;begin < end; begin++)
    {
      if (mysql_real_query(mysql, *begin, (unsigned long)strlen(*begin)))
        goto error;

    /* check if query produced a result set */
      do {
        MYSQL_RES *res;
        if ((res= mysql_use_result(mysql)))
          mysql_free_result(res);
      } while (!mysql_next_result(mysql));
    }
    mysql->options.reconnect= save_reconnect;
  }

  strcpy(mysql->net.sqlstate, "00000");

  /* connection established, apply timeouts */
  ma_pvio_set_timeout(mysql->net.pvio, PVIO_READ_TIMEOUT, mysql->options.read_timeout);
  ma_pvio_set_timeout(mysql->net.pvio, PVIO_WRITE_TIMEOUT, mysql->options.write_timeout);
  return(mysql);

error:
  /* Free alloced memory */
  end_server(mysql);
  /* only free the allocated memory, user needs to call mysql_close */
  mysql_close_memory(mysql);
  if (!(client_flag & CLIENT_REMEMBER_OPTIONS) &&
      !mysql->options.extension->async_context)
    mysql_close_options(mysql);
  return(0);
}