mysql_get_optionv(MYSQL *mysql, enum mysql_option option, void *arg, ...)
{
  va_list ap;

  va_start(ap, arg);

  switch(option) {
  case MYSQL_OPT_CONNECT_TIMEOUT:
    *((uint *)arg)= mysql->options.connect_timeout;
    break;
  case MYSQL_OPT_COMPRESS:
    *((my_bool *)arg)= mysql->options.compress;
    break;
  case MYSQL_OPT_NAMED_PIPE:
    *((my_bool *)arg)= mysql->options.named_pipe;
    break;
  case MYSQL_OPT_LOCAL_INFILE:			/* Allow LOAD DATA LOCAL ?*/
    *((uint *)arg)= test(mysql->options.client_flag & CLIENT_LOCAL_FILES);
    break;
  case MYSQL_INIT_COMMAND:
    /* mysql_get_optionsv(mysql, MYSQL_INIT_COMMAND, commands, elements) */
    {
      unsigned int *elements;
      if (arg)
        *((char **)arg)= mysql->options.init_command ? mysql->options.init_command->buffer : NULL;
      if ((elements= va_arg(ap, unsigned int *)))
        *elements= mysql->options.init_command ? mysql->options.init_command->elements : 0;
    }
    break;
  case MYSQL_READ_DEFAULT_FILE:
    *((char **)arg)= mysql->options.my_cnf_file;
    break;
  case MYSQL_READ_DEFAULT_GROUP:
    *((char **)arg)= mysql->options.my_cnf_group;
    break;
  case MYSQL_SET_CHARSET_DIR:
    /* not supported in this version. Since all character sets
       are internally available, we don't throw an error */
    *((char **)arg)= NULL;
    break;
  case MYSQL_SET_CHARSET_NAME:
    if (mysql->charset)
      *((const char **)arg)= mysql->charset->csname;
    else
      *((char **)arg)= mysql->options.charset_name;
    break;
  case MYSQL_OPT_RECONNECT:
    *((my_bool *)arg)= mysql->options.reconnect;
    break;
  case MYSQL_OPT_PROTOCOL:
    *((uint *)arg)= mysql->options.protocol;
    break;
  case MYSQL_OPT_READ_TIMEOUT:
    *((uint *)arg)= mysql->options.read_timeout;
    break;
  case MYSQL_OPT_WRITE_TIMEOUT:
    *((uint *)arg)= mysql->options.write_timeout;
    break;
  case MYSQL_REPORT_DATA_TRUNCATION:
    *((my_bool *)arg)= mysql->options.report_data_truncation;
    break;
  case MYSQL_PROGRESS_CALLBACK:
    *((void (**)(const MYSQL *, uint, uint, double, const char *, uint))arg)=
       mysql->options.extension ?  mysql->options.extension->report_progress : NULL;
    break;
  case MYSQL_SERVER_PUBLIC_KEY:
    *((char **)arg)= mysql->options.extension ?
      mysql->options.extension->server_public_key : NULL;
    break;
  case MYSQL_PLUGIN_DIR:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->plugin_dir : NULL;
    break;
  case MYSQL_DEFAULT_AUTH:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->default_auth : NULL;
    break;
  case MYSQL_OPT_NONBLOCK:
    *((my_bool *)arg)= test(mysql->options.extension && mysql->options.extension->async_context);
    break;
  case MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS:
    *((my_bool *)arg)= test(mysql->options.client_flag & CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS);
    break;
  case MYSQL_OPT_SSL_ENFORCE:
    *((my_bool *)arg)= mysql->options.use_ssl;
    break;
  case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
    *((my_bool *)arg)= test(mysql->options.client_flag & CLIENT_SSL_VERIFY_SERVER_CERT);
    break;
  case MYSQL_OPT_SSL_KEY:
    *((char **)arg)= mysql->options.ssl_key;
    break;
  case MYSQL_OPT_SSL_CERT:
    *((char **)arg)= mysql->options.ssl_cert;
    break;
  case MYSQL_OPT_SSL_CA:
    *((char **)arg)= mysql->options.ssl_ca;
    break;
  case MYSQL_OPT_SSL_CAPATH:
    *((char **)arg)= mysql->options.ssl_capath;
    break;
  case MYSQL_OPT_SSL_CIPHER:
    *((char **)arg)= mysql->options.ssl_cipher;
    break;
  case MYSQL_OPT_SSL_CRL:
    *((char **)arg)= mysql->options.extension ? mysql->options.ssl_cipher : NULL;
    break;
  case MYSQL_OPT_SSL_CRLPATH:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->ssl_crlpath : NULL;
    break;
  case MYSQL_OPT_CONNECT_ATTRS:
    /* mysql_get_optionsv(mysql, MYSQL_OPT_CONNECT_ATTRS, keys, vals, elements) */
    {
      unsigned int i, *elements;
      char **key= NULL;
      void *arg1;
      char **val= NULL;

      if (arg)
        key= *(char ***)arg;

      arg1= va_arg(ap, char **);
      if (arg1)
        val= *(char ***)arg1;

      if (!(elements= va_arg(ap, unsigned int *)))
        goto error;

      *elements= 0;

      if (!mysql->options.extension ||
          !hash_inited(&mysql->options.extension->connect_attrs))
        break;

      *elements= mysql->options.extension->connect_attrs.records;

      if (val || key)
      {
        for (i=0; i < *elements; i++)
        {
          uchar *p= hash_element(&mysql->options.extension->connect_attrs, i);
          if (key)
            key[i]= (char *)p;
          p+= strlen((char *)p) + 1;
          if (val)
            val[i]= (char *)p;
        }
      }
    }
    break;
  case MYSQL_OPT_MAX_ALLOWED_PACKET:
    *((unsigned long *)arg)= (mysql) ? mysql->options.max_allowed_packet :
                                       max_allowed_packet;
    break;
  case MYSQL_OPT_NET_BUFFER_LENGTH:
    *((unsigned long *)arg)= net_buffer_length;
    break;
  case MYSQL_SECURE_AUTH:
    *((my_bool *)arg)= mysql->options.secure_auth;
    break;
  case MYSQL_OPT_BIND:
    *((char **)arg)= mysql->options.bind_address;
    break;
  case MARIADB_OPT_TLS_CIPHER_STRENGTH:
    *((unsigned int *)arg) = mysql->options.extension ? mysql->options.extension->tls_cipher_strength : 0;
    break;
  case MARIADB_OPT_SSL_FP:
  case MARIADB_OPT_TLS_PEER_FP:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->tls_fp : NULL;
    break;
  case MARIADB_OPT_SSL_FP_LIST:
  case MARIADB_OPT_TLS_PEER_FP_LIST:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->tls_fp_list : NULL;
    break;
  case MARIADB_OPT_TLS_PASSPHRASE:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->tls_pw : NULL;
    break;
  case MARIADB_OPT_CONNECTION_READ_ONLY:
    *((my_bool *)arg)= mysql->options.extension ? mysql->options.extension->read_only : 0;
    break;
  case MARIADB_OPT_USERDATA:
    /* nysql_get_optionv(mysql, MARIADB_OPT_USERDATA, key, value) */
    {
      uchar *p;
      void *data= va_arg(ap, void *);
      char *key= (char *)arg;
      if (key && data && mysql->options.extension && hash_inited(&mysql->options.extension->userdata) &&
          (p= (uchar *)hash_search(&mysql->options.extension->userdata, (uchar *)key,
                      (uint)strlen((char *)key))))
      {
        p+= strlen(key) + 1;
        *((void **)data)= *((void **)p);
        break;
      }
      if (data)
        *((void **)data)= NULL;
    }
    break;
  case MARIADB_OPT_CONNECTION_HANDLER:
    *((char **)arg)= mysql->options.extension ? mysql->options.extension->connection_handler : NULL;
    break;
  case MARIADB_OPT_IO_WAIT:
    *((int(**)(my_socket, my_bool, int))arg) = mysql->options.extension ? mysql->options.extension->io_wait : NULL;
    break;
  default:
    va_end(ap);
    SET_CLIENT_ERROR(mysql, CR_NOT_IMPLEMENTED, SQLSTATE_UNKNOWN, 0);
    return(1);
  }
  va_end(ap);
  return(0);
error:
  va_end(ap);
  return(1);
}