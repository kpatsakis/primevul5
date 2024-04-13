int STDCALL mysql_reset_connection(MYSQL *mysql)
{
  int rc;

  /* check if connection handler is active */
  if (IS_CONNHDLR_ACTIVE(mysql))
  {
    if (mysql->extension->conn_hdlr->plugin && mysql->extension->conn_hdlr->plugin->reset)
      return(mysql->extension->conn_hdlr->plugin->reset(mysql));
  }

  /* skip result sets */
  if (mysql->status == MYSQL_STATUS_USE_RESULT ||
      mysql->status == MYSQL_STATUS_GET_RESULT ||
      mysql->status & SERVER_MORE_RESULTS_EXIST)
  {
    mthd_my_skip_result(mysql);
    mysql->status= MYSQL_STATUS_READY;
  }

  rc= ma_simple_command(mysql, COM_RESET_CONNECTION, 0, 0, 0, 0);
  if (rc && mysql->options.reconnect)
  {
    /* There is no big sense in resetting but we need reconnect */
    rc= ma_simple_command(mysql, COM_RESET_CONNECTION,0,0,0,0);
  }
  if (rc)
    return 1;

  /* reset the connection in all active statements */
  ma_invalidate_stmts(mysql, "mysql_reset_connection()");
  free_old_query(mysql);
  mysql->status= MYSQL_STATUS_READY;
  mysql->affected_rows= ~(my_ulonglong)0;
  mysql->insert_id= 0;
  return 0;
}