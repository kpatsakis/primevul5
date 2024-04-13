static void mysql_close_options(MYSQL *mysql)
{
  if (mysql->options.init_command)
  {
    char **begin= (char **)mysql->options.init_command->buffer;
    char **end= begin + mysql->options.init_command->elements;

    for (;begin < end; begin++)
      free(*begin);
    ma_delete_dynamic(mysql->options.init_command);
    free(mysql->options.init_command);
  }
  free(mysql->options.user);
  free(mysql->options.host);
  free(mysql->options.password);
  free(mysql->options.unix_socket);
  free(mysql->options.db);
  free(mysql->options.my_cnf_file);
  free(mysql->options.my_cnf_group);
  free(mysql->options.charset_dir);
  free(mysql->options.charset_name);
  free(mysql->options.bind_address);
  free(mysql->options.ssl_key);
  free(mysql->options.ssl_cert);
  free(mysql->options.ssl_ca);
  free(mysql->options.ssl_capath);
  free(mysql->options.ssl_cipher);

  if (mysql->options.extension)
  {
    struct mysql_async_context *ctxt;
    if ((ctxt = mysql->options.extension->async_context))
    {
      my_context_destroy(&ctxt->async_context);
      free(ctxt);
      mysql->options.extension->async_context= 0;
    }
    free(mysql->options.extension->plugin_dir);
    free(mysql->options.extension->default_auth);
    free(mysql->options.extension->db_driver);
    free(mysql->options.extension->ssl_crl);
    free(mysql->options.extension->ssl_crlpath);
    free(mysql->options.extension->tls_fp);
    free(mysql->options.extension->tls_fp_list);
    free(mysql->options.extension->tls_pw);
    free(mysql->options.extension->tls_version);
    free(mysql->options.extension->url);
    free(mysql->options.extension->connection_handler);
    if(hash_inited(&mysql->options.extension->connect_attrs))
      hash_free(&mysql->options.extension->connect_attrs);
    if (hash_inited(&mysql->options.extension->userdata))
      hash_free(&mysql->options.extension->userdata);

  }
  free(mysql->options.extension);
  /* clear all pointer */
  memset(&mysql->options, 0, sizeof(mysql->options));
}