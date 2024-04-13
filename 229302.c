my_bool mariadb_get_infov(MYSQL *mysql, enum mariadb_value value, void *arg, ...)
{
  va_list ap;

  va_start(ap, arg);

  switch(value) {
  case MARIADB_MAX_ALLOWED_PACKET:
    *((size_t *)arg)= (size_t)max_allowed_packet;
    break;
  case MARIADB_NET_BUFFER_LENGTH:
    *((size_t *)arg)= (size_t)net_buffer_length;
    break;
  case MARIADB_CONNECTION_ERROR_ID:
    if (!mysql)
      goto error;
    *((unsigned int *)arg)= mysql->net.last_errno;
    break;
  case MARIADB_CONNECTION_ERROR:
    if (!mysql)
      goto error;
    *((char **)arg)= mysql->net.last_error;
    break;
  case MARIADB_CONNECTION_SQLSTATE:
    if (!mysql)
      goto error;
    *((char **)arg)= mysql->net.sqlstate;
    break;
  case MARIADB_CONNECTION_TLS_VERSION:
    #ifdef HAVE_TLS
    if (mysql && mysql->net.pvio && mysql->net.pvio->ctls)
      *((char **)arg)= (char *)ma_pvio_tls_get_protocol_version(mysql->net.pvio->ctls);
    else
    #endif
      goto error;
    break;
  case MARIADB_CONNECTION_TLS_VERSION_ID:
    #ifdef HAVE_TLS
    if (mysql && mysql->net.pvio && mysql->net.pvio->ctls)
      *((unsigned int *)arg)= ma_pvio_tls_get_protocol_version_id(mysql->net.pvio->ctls);
    else
    #endif
      goto error;
    break;
  case MARIADB_TLS_LIBRARY:
#ifdef HAVE_TLS
    *((const char **)arg)= tls_library_version;
#else
    *((const char **)arg)= "Off";
#endif
    break;
  case MARIADB_CLIENT_VERSION:
    *((const char **)arg)= MARIADB_CLIENT_VERSION_STR;
    break;
  case MARIADB_CLIENT_VERSION_ID:
    *((size_t *)arg)= MARIADB_VERSION_ID;
    break;
  case MARIADB_CONNECTION_SERVER_VERSION:
    if (mysql)
      *((char **)arg)= mysql->server_version;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SERVER_TYPE:
    if (mysql)
      *((const char **)arg)= mariadb_connection(mysql) ? "MariaDB" : "MySQL";
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SERVER_VERSION_ID:
    if (mysql)
      *((size_t *)arg)= mariadb_server_version_id(mysql);
    else
      goto error;
    break;
  case MARIADB_CONNECTION_PROTOCOL_VERSION_ID:
    if (mysql)
      *((unsigned int *)arg)= mysql->protocol_version;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_MARIADB_CHARSET_INFO:
    if (mysql)
      mariadb_get_charset_info(mysql, (MY_CHARSET_INFO *)arg);
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SOCKET:
    if (mysql)
      *((my_socket *)arg)= mariadb_get_socket(mysql);
    else
      goto error;
    break;
  case MARIADB_CONNECTION_TYPE:
    if (mysql  && mysql->net.pvio)
      *((int *)arg)= (int)mysql->net.pvio->type;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_ASYNC_TIMEOUT_MS:
    if (mysql && mysql->options.extension && mysql->options.extension->async_context)
      *((unsigned int *)arg)= mysql->options.extension->async_context->timeout_value;
    break;
  case MARIADB_CONNECTION_ASYNC_TIMEOUT:
    if (mysql && mysql->options.extension && mysql->options.extension->async_context)
    {
      unsigned int timeout= mysql->options.extension->async_context->timeout_value;
      if (timeout > UINT_MAX - 999)
        *((unsigned int *)arg)= (timeout - 1)/1000 + 1;
      else
        *((unsigned int *)arg)= (timeout+999)/1000;
    }
    break;
  case MARIADB_CHARSET_NAME:
    {
      char *name;
      name= va_arg(ap, char *);
      if (name)
        *((MARIADB_CHARSET_INFO **)arg)= (MARIADB_CHARSET_INFO *)mysql_find_charset_name(name);
      else
        goto error;
    }
    break;
  case MARIADB_CHARSET_ID:
    {
      unsigned int nr;
      nr= va_arg(ap, unsigned int);
      *((MARIADB_CHARSET_INFO **)arg)= (MARIADB_CHARSET_INFO *)mysql_find_charset_nr(nr);
    }
    break;
  case MARIADB_CONNECTION_SSL_CIPHER:
    #ifdef HAVE_TLS
    if (mysql && mysql->net.pvio && mysql->net.pvio->ctls)
      *((char **)arg)= (char *)ma_pvio_tls_cipher(mysql->net.pvio->ctls);
    else
    #endif
      goto error;
    break;
  case MARIADB_CLIENT_ERRORS:
    *((char ***)arg)= (char **)client_errors;
    break;
  case MARIADB_CONNECTION_INFO:
    if (mysql)
      *((char **)arg)= (char *)mysql->info;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_PVIO_TYPE:
    if (mysql && mysql->net.pvio)
      *((unsigned int *)arg)= (unsigned int)mysql->net.pvio->type;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SCHEMA:
    if (mysql)
      *((char **)arg)= mysql->db;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_USER:
    if (mysql)
      *((char **)arg)= mysql->user;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_PORT:
    if (mysql)
      *((unsigned int *)arg)= mysql->port;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_UNIX_SOCKET:
    if (mysql)
      *((char **)arg)= mysql->unix_socket;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_HOST:
    if (mysql)
      *((char **)arg)= mysql->host;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SERVER_STATUS:
    if (mysql)
      *((unsigned int *)arg)= mysql->server_status;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_SERVER_CAPABILITIES:
    if (mysql)
      *((unsigned long *)arg)= mysql->server_capabilities;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_EXTENDED_SERVER_CAPABILITIES:
    if (mysql)
      *((unsigned long *)arg)= mysql->extension->mariadb_server_capabilities;
    else
      goto error;
    break;
  case MARIADB_CONNECTION_CLIENT_CAPABILITIES:
    if (mysql)
      *((unsigned long *)arg)= mysql->client_flag;
    else
      goto error;
    break;
  default:
    va_end(ap);
    return(-1);
  }
  va_end(ap);
  return(0);
error:
  va_end(ap);
  return(-1);
}