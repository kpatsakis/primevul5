mysql_optionsv(MYSQL *mysql,enum mysql_option option, ...)
{
  va_list ap;
  void *arg1;
  size_t stacksize;
  struct mysql_async_context *ctxt;

  va_start(ap, option);

  arg1= va_arg(ap, void *);

  switch (option) {
  case MYSQL_OPT_CONNECT_TIMEOUT:
    mysql->options.connect_timeout= *(uint*) arg1;
    break;
  case MYSQL_OPT_COMPRESS:
    mysql->options.compress= 1;			/* Remember for connect */
    mysql->options.client_flag|= CLIENT_COMPRESS;
    break;
  case MYSQL_OPT_NAMED_PIPE:
    mysql->options.named_pipe=1;		/* Force named pipe */
    break;
  case MYSQL_OPT_LOCAL_INFILE:			/* Allow LOAD DATA LOCAL ?*/
    if (!arg1 || test(*(unsigned int*) arg1))
      mysql->options.client_flag|= CLIENT_LOCAL_FILES;
    else
      mysql->options.client_flag&= ~CLIENT_LOCAL_FILES;
    if (arg1) {
      CHECK_OPT_EXTENSION_SET(&mysql->options);
      mysql->extension->auto_local_infile= *(uint*)arg1 == LOCAL_INFILE_MODE_AUTO
                                           ? WAIT_FOR_QUERY : ALWAYS_ACCEPT;
    }
    break;
  case MYSQL_INIT_COMMAND:
    options_add_initcommand(&mysql->options, (char *)arg1);
    break;
  case MYSQL_READ_DEFAULT_FILE:
    OPT_SET_VALUE_STR(&mysql->options, my_cnf_file, (char *)arg1);
    break;
  case MYSQL_READ_DEFAULT_GROUP:
    OPT_SET_VALUE_STR(&mysql->options, my_cnf_group, arg1 ? (char *)arg1 : "");
    break;
  case MYSQL_SET_CHARSET_DIR:
    OPT_SET_VALUE_STR(&mysql->options, charset_dir, arg1);
    break;
  case MYSQL_SET_CHARSET_NAME:
    OPT_SET_VALUE_STR(&mysql->options, charset_name, arg1);
    break;
  case MYSQL_OPT_RECONNECT:
    mysql->options.reconnect= *(my_bool *)arg1;
    break;
  case MYSQL_OPT_PROTOCOL:
    mysql->options.protocol= *((uint *)arg1);
    break;
#ifdef _WIN32
  case MYSQL_SHARED_MEMORY_BASE_NAME:
    OPT_SET_VALUE_STR(&mysql->options, shared_memory_base_name, arg1);
    break;
#endif
  case MYSQL_OPT_READ_TIMEOUT:
    mysql->options.read_timeout= *(uint *)arg1;
    break;
  case MYSQL_OPT_WRITE_TIMEOUT:
    mysql->options.write_timeout= *(uint *)arg1;
    break;
  case MYSQL_REPORT_DATA_TRUNCATION:
    mysql->options.report_data_truncation= *(my_bool *)arg1;
    break;
  case MYSQL_PROGRESS_CALLBACK:
    CHECK_OPT_EXTENSION_SET(&mysql->options);
    if (mysql->options.extension)
      mysql->options.extension->report_progress=
        (void (*)(const MYSQL *, uint, uint, double, const char *, uint)) arg1;
    break;
  case MYSQL_SERVER_PUBLIC_KEY:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, server_public_key, (char *)arg1);
    break;
  case MYSQL_PLUGIN_DIR:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, plugin_dir, (char *)arg1);
    break;
  case MYSQL_DEFAULT_AUTH:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, default_auth, (char *)arg1);
    break;
  case MYSQL_OPT_NONBLOCK:
    if (mysql->options.extension &&
        (ctxt = mysql->options.extension->async_context) != 0)
    {
      /*
        We must not allow changing the stack size while a non-blocking call is
        suspended (as the stack is then in use).
      */
      if (ctxt->suspended)
        goto end;
      my_context_destroy(&ctxt->async_context);
      free(ctxt);
    }
    if (!(ctxt= (struct mysql_async_context *)
          calloc(1, sizeof(*ctxt))))
    {
      SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
      goto end;
    }
    stacksize= 0;
    if (arg1)
      stacksize= *(const size_t *)arg1;
    if (!stacksize)
      stacksize= ASYNC_CONTEXT_DEFAULT_STACK_SIZE;
    if (my_context_init(&ctxt->async_context, stacksize))
    {
      free(ctxt);
      goto end;
    }
    if (!mysql->options.extension)
      if(!(mysql->options.extension= (struct st_mysql_options_extension *)
        calloc(1, sizeof(struct st_mysql_options_extension))))
      {
        free(ctxt);
        SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
        goto end;
      }
    mysql->options.extension->async_context= ctxt;
    break;
  case MYSQL_OPT_MAX_ALLOWED_PACKET:
    if (mysql)
      mysql->options.max_allowed_packet= (unsigned long)(*(size_t *)arg1);
    else
      max_allowed_packet= (unsigned long)(*(size_t *)arg1);
    break;
  case MYSQL_OPT_NET_BUFFER_LENGTH:
    net_buffer_length= (unsigned long)(*(size_t *)arg1);
    break;
  case MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS:
    if (*(my_bool *)arg1)
      mysql->options.client_flag |= CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;
    else
      mysql->options.client_flag &= ~CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;
    break;
  case MYSQL_OPT_SSL_ENFORCE:
    mysql->options.use_ssl= (*(my_bool *)arg1);
    break;
  case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
    if (*(my_bool *)arg1)
      mysql->options.client_flag |= CLIENT_SSL_VERIFY_SERVER_CERT;
    else
      mysql->options.client_flag &= ~CLIENT_SSL_VERIFY_SERVER_CERT;
    break;
  case MYSQL_OPT_SSL_KEY:
    OPT_SET_VALUE_STR(&mysql->options, ssl_key, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CERT:
    OPT_SET_VALUE_STR(&mysql->options, ssl_cert, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CA:
    OPT_SET_VALUE_STR(&mysql->options, ssl_ca, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CAPATH:
    OPT_SET_VALUE_STR(&mysql->options, ssl_capath, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CIPHER:
    OPT_SET_VALUE_STR(&mysql->options, ssl_cipher, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CRL:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, ssl_crl, (char *)arg1);
    break;
  case MYSQL_OPT_SSL_CRLPATH:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, ssl_crlpath, (char *)arg1);
    break;
  case MYSQL_OPT_CONNECT_ATTR_DELETE:
    {
      uchar *h;
      CHECK_OPT_EXTENSION_SET(&mysql->options);
      if (hash_inited(&mysql->options.extension->connect_attrs) &&
          (h= (uchar *)hash_search(&mysql->options.extension->connect_attrs, (uchar *)arg1,
                      arg1 ? (uint)strlen((char *)arg1) : 0)))
      {
        uchar *p= h;
        size_t key_len= strlen((char *)p);
        mysql->options.extension->connect_attrs_len-= key_len + get_store_length(key_len);
        p+= key_len + 1;
        key_len= strlen((char *)p);
        mysql->options.extension->connect_attrs_len-= key_len + get_store_length(key_len);
        hash_delete(&mysql->options.extension->connect_attrs, h);
      }

    }
    break;
  case MYSQL_OPT_CONNECT_ATTR_RESET:
    CHECK_OPT_EXTENSION_SET(&mysql->options);
    if (hash_inited(&mysql->options.extension->connect_attrs))
    {
      hash_free(&mysql->options.extension->connect_attrs);
      mysql->options.extension->connect_attrs_len= 0;
    }
    break;
  case MARIADB_OPT_CONNECTION_HANDLER:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, connection_handler, (char *)arg1);
    break;
  case MARIADB_OPT_PORT:
    OPT_SET_VALUE_INT(&mysql->options, port, *((uint *)arg1));
    break;
  case MARIADB_OPT_UNIXSOCKET:
    OPT_SET_VALUE_STR(&mysql->options, unix_socket, arg1);
    break;
  case MARIADB_OPT_USER:
    OPT_SET_VALUE_STR(&mysql->options, user, arg1);
    break;
  case MARIADB_OPT_HOST:
    OPT_SET_VALUE_STR(&mysql->options, host, arg1);
    break;
  case MARIADB_OPT_SCHEMA:
    OPT_SET_VALUE_STR(&mysql->options, db, arg1);
    break;
  case MARIADB_OPT_DEBUG:
    break;
  case MARIADB_OPT_FOUND_ROWS:
    mysql->options.client_flag|= CLIENT_FOUND_ROWS;
    break;
  case MARIADB_OPT_INTERACTIVE:
    mysql->options.client_flag|= CLIENT_INTERACTIVE;
    break;
  case MARIADB_OPT_MULTI_RESULTS:
    mysql->options.client_flag|= CLIENT_MULTI_RESULTS;
    break;
  case MARIADB_OPT_MULTI_STATEMENTS:
    mysql->options.client_flag|= CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS;
    break;
  case MARIADB_OPT_PASSWORD:
    OPT_SET_VALUE_STR(&mysql->options, password, arg1);
    break;
  case MARIADB_OPT_USERDATA:
    {
      void *data= va_arg(ap, void *);
      uchar *buffer, *p;
      char *key= (char *)arg1;

      if (!key || !data)
      {
        SET_CLIENT_ERROR(mysql, CR_INVALID_PARAMETER_NO, SQLSTATE_UNKNOWN, 0);
        goto end;
      }

      CHECK_OPT_EXTENSION_SET(&mysql->options);
      if (!hash_inited(&mysql->options.extension->userdata))
      {
        if (_hash_init(&mysql->options.extension->userdata,
                       0, 0, 0, ma_get_hash_keyval, ma_int_hash_free, 0))
        {
          SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
          goto end;
        }
      }
      /* check if key is already in buffer */
      p= (uchar *)hash_search(&mysql->options.extension->userdata, 
                              (uchar *)key,
                              (uint)strlen(key));
      if (p)
      {
        p+= strlen(key) + 1;
        memcpy(p, &data, sizeof(void *));
        break;
      }

      if (!(buffer= (uchar *)malloc(strlen(key) + 1 + sizeof(void *))))
      {
        SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
        goto end;
      }

      p= buffer;
      strcpy((char *)p, key);
      p+= strlen(key) + 1;
      memcpy(p, &data, sizeof(void *));

      if (hash_insert(&mysql->options.extension->userdata, buffer))
      {
        free(buffer);
        SET_CLIENT_ERROR(mysql, CR_INVALID_PARAMETER_NO, SQLSTATE_UNKNOWN, 0);
        goto end;
      }
    }
    break;
  case MYSQL_OPT_CONNECT_ATTR_ADD:
    {
      uchar *buffer;
      void *arg2= va_arg(ap, void *);
      size_t storage_len, key_len= arg1 ? strlen((char *)arg1) : 0,
             value_len= arg2 ? strlen((char *)arg2) : 0;
      if (!key_len || !value_len)
      {
        SET_CLIENT_ERROR(mysql, CR_INVALID_PARAMETER_NO, SQLSTATE_UNKNOWN, 0);
        goto end;
      }
      storage_len= key_len + value_len +
                   get_store_length(key_len) +
                   get_store_length(value_len);

      /* since we store terminating zero character in hash, we need
       * to increase lengths */
      key_len++;
      value_len++;

      CHECK_OPT_EXTENSION_SET(&mysql->options);
      if (!key_len ||
          storage_len + mysql->options.extension->connect_attrs_len > 0xFFFF)
      {
        SET_CLIENT_ERROR(mysql, CR_INVALID_PARAMETER_NO, SQLSTATE_UNKNOWN, 0);
        goto end;
      }

      if (!hash_inited(&mysql->options.extension->connect_attrs))
      {
        if (_hash_init(&mysql->options.extension->connect_attrs,
                       0, 0, 0, ma_get_hash_keyval, ma_int_hash_free, 0))
        {
          SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
          goto end;
        }
      }
      if ((buffer= (uchar *)malloc(key_len + value_len)))
      {
        uchar *p= buffer;
        strcpy((char *)p, arg1);
        p+= (strlen(arg1) + 1);
        if (arg2)
          strcpy((char *)p, arg2);

        if (hash_insert(&mysql->options.extension->connect_attrs, buffer))
        {
          free(buffer);
          SET_CLIENT_ERROR(mysql, CR_INVALID_PARAMETER_NO, SQLSTATE_UNKNOWN, 0);
          goto end;
        }
        mysql->options.extension->connect_attrs_len+= storage_len;
      }
      else
      {
        SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
        goto end;
      }
    }
    break;
  case MYSQL_ENABLE_CLEARTEXT_PLUGIN:
    break;
  case MYSQL_SECURE_AUTH:
    mysql->options.secure_auth= *(my_bool *)arg1;
    break;
  case MYSQL_OPT_BIND:
    OPT_SET_VALUE_STR(&mysql->options, bind_address, arg1);
    break;
  case MARIADB_OPT_TLS_CIPHER_STRENGTH:
    OPT_SET_EXTENDED_VALUE_INT(&mysql->options, tls_cipher_strength, *((unsigned int *)arg1));
    break;
  case MARIADB_OPT_SSL_FP:
  case MARIADB_OPT_TLS_PEER_FP:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, tls_fp, (char *)arg1);
    mysql->options.use_ssl= 1;
    break;
  case MARIADB_OPT_SSL_FP_LIST:
  case MARIADB_OPT_TLS_PEER_FP_LIST:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, tls_fp_list, (char *)arg1);
    mysql->options.use_ssl= 1;
    break;
  case MARIADB_OPT_TLS_PASSPHRASE:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, tls_pw, (char *)arg1);
    break;
  case MARIADB_OPT_CONNECTION_READ_ONLY:
    OPT_SET_EXTENDED_VALUE_INT(&mysql->options, read_only, *(my_bool *)arg1);
    break;
  case MARIADB_OPT_PROXY_HEADER:
    {
    size_t arg2 = va_arg(ap, size_t);
    OPT_SET_EXTENDED_VALUE(&mysql->options, proxy_header, (char *)arg1);
    OPT_SET_EXTENDED_VALUE(&mysql->options, proxy_header_len, arg2);
    }
    break;
  case MARIADB_OPT_TLS_VERSION:
  case MYSQL_OPT_TLS_VERSION:
    OPT_SET_EXTENDED_VALUE_STR(&mysql->options, tls_version, (char *)arg1);
    break;
  case MARIADB_OPT_IO_WAIT:
    CHECK_OPT_EXTENSION_SET(&mysql->options);
    mysql->options.extension->io_wait = (int(*)(my_socket, my_bool, int))arg1;
    break;
  default:
    va_end(ap);
    SET_CLIENT_ERROR(mysql, CR_NOT_IMPLEMENTED, SQLSTATE_UNKNOWN, 0);
    return(1);
  }
  va_end(ap);
  return(0);
end:
  va_end(ap);
  return(1);
}