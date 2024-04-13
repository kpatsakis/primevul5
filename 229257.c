mysql_close(MYSQL *mysql)
{
  if (mysql)					/* Some simple safety */
  {
    if (mysql->extension && mysql->extension->conn_hdlr)
    {
      MA_CONNECTION_HANDLER *p= mysql->extension->conn_hdlr;
      if (p->plugin->close)
        p->plugin->close(mysql);
      free(p);
      /* Fix for CONC-294: Since we already called plugin->close function
         we need to prevent that mysql_close_slow_part (which sends COM_QUIT
         to the server) will be handled by plugin again. */
      mysql->extension->conn_hdlr= NULL;
    }

    if (mysql->methods)
      mysql->methods->db_close(mysql);

    /* reset the connection in all active statements */
    ma_invalidate_stmts(mysql, "mysql_close()");

    mysql_close_memory(mysql);
    mysql_close_options(mysql);
    ma_clear_session_state(mysql);

    if (mysql->net.extension)
      free(mysql->net.extension);

    mysql->host_info=mysql->user=mysql->passwd=mysql->db=0;

    /* Clear pointers for better safety */
    memset((char*) &mysql->options, 0, sizeof(mysql->options));

    if (mysql->extension)
      free(mysql->extension);

    mysql->net.pvio= 0;
    if (mysql->free_me)
      free(mysql);
  }
  return;
}