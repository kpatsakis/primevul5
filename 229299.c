my_bool STDCALL mariadb_reconnect(MYSQL *mysql)
{
  MYSQL tmp_mysql;
  struct my_hook_data hook_data;
  struct mysql_async_context *ctxt= NULL;
  LIST *li_stmt= mysql->stmts;

  /* check if connection handler is active */
  if (IS_CONNHDLR_ACTIVE(mysql))
  {
    if (mysql->extension->conn_hdlr->plugin && mysql->extension->conn_hdlr->plugin->reconnect)
      return(mysql->extension->conn_hdlr->plugin->reconnect(mysql));
  }

  if (!mysql->options.reconnect ||
      (mysql->server_status & SERVER_STATUS_IN_TRANS) || !mysql->host_info)
  {
   /* Allow reconnect next time */
    mysql->server_status&= ~SERVER_STATUS_IN_TRANS;
    my_set_error(mysql, CR_SERVER_GONE_ERROR, SQLSTATE_UNKNOWN, 0);
    return(1);
  }

  mysql_init(&tmp_mysql);
  tmp_mysql.free_me= 0;
  tmp_mysql.options=mysql->options;
  if (mysql->extension->conn_hdlr)
  {
    tmp_mysql.extension->conn_hdlr= mysql->extension->conn_hdlr;
    mysql->extension->conn_hdlr= 0;
  }

  /* don't reread options from configuration files */
  tmp_mysql.options.my_cnf_group= tmp_mysql.options.my_cnf_file= NULL;
  if (IS_MYSQL_ASYNC_ACTIVE(mysql))
  {
    ctxt= mysql->options.extension->async_context;
    hook_data.orig_mysql= mysql;
    hook_data.new_mysql= &tmp_mysql;
    hook_data.orig_pvio= mysql->net.pvio;
    my_context_install_suspend_resume_hook(ctxt, my_suspend_hook, &hook_data);
  }

  if (!mysql_real_connect(&tmp_mysql,mysql->host,mysql->user,mysql->passwd,
			  mysql->db, mysql->port, mysql->unix_socket,
			  mysql->client_flag | CLIENT_REMEMBER_OPTIONS) ||
      mysql_set_character_set(&tmp_mysql, mysql->charset->csname))
  {
    if (ctxt)
      my_context_install_suspend_resume_hook(ctxt, NULL, NULL);
    /* don't free options (CONC-118) */
    memset(&tmp_mysql.options, 0, sizeof(struct st_mysql_options));
    my_set_error(mysql, tmp_mysql.net.last_errno,
                        tmp_mysql.net.sqlstate,
                        tmp_mysql.net.last_error);
    mysql_close(&tmp_mysql);
    return(1);
  }

  for (;li_stmt;li_stmt= li_stmt->next)
  {
    MYSQL_STMT *stmt= (MYSQL_STMT *)li_stmt->data;

    if (stmt->state != MYSQL_STMT_INITTED)
    {
      stmt->state= MYSQL_STMT_INITTED;
      SET_CLIENT_STMT_ERROR(stmt, CR_SERVER_LOST, SQLSTATE_UNKNOWN, 0);
    }
  }

  tmp_mysql.free_me= mysql->free_me;
  tmp_mysql.stmts= mysql->stmts;
  mysql->stmts= NULL;

  if (ctxt)
    my_context_install_suspend_resume_hook(ctxt, NULL, NULL);
  /* Don't free options, we moved them to tmp_mysql */
  memset(&mysql->options, 0, sizeof(mysql->options));
  mysql->free_me=0;
  mysql_close(mysql);
  *mysql=tmp_mysql;
  mysql->net.pvio->mysql= mysql;
  ma_net_clear(&mysql->net);
  mysql->affected_rows= ~(unsigned long long) 0;
  mysql->info= 0;
  return(0);
}