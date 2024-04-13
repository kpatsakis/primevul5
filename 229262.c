mthd_my_send_cmd(MYSQL *mysql,enum enum_server_command command, const char *arg,
	       size_t length, my_bool skipp_check, void *opt_arg)
{
  NET *net= &mysql->net;
  int result= -1;
  if (mysql->net.pvio == 0)
  {
    /* Do reconnect if possible */
    if (mariadb_reconnect(mysql))
      return(1);
  }
  if (mysql->status != MYSQL_STATUS_READY ||
      mysql->server_status & SERVER_MORE_RESULTS_EXIST)
  {
    SET_CLIENT_ERROR(mysql, CR_COMMANDS_OUT_OF_SYNC, SQLSTATE_UNKNOWN, 0);
    goto end;
  }

  if (IS_CONNHDLR_ACTIVE(mysql))
  {
    result= mysql->extension->conn_hdlr->plugin->set_connection(mysql, command, arg, length, skipp_check, opt_arg);
    if (result== -1)
      return(result);
  }

  CLEAR_CLIENT_ERROR(mysql);

  mysql->info=0;
  mysql->affected_rows= ~(unsigned long long) 0;
  ma_net_clear(net);			/* Clear receive buffer */
  if (!arg)
    arg="";

  if (net->extension->multi_status== COM_MULTI_ENABLED)
  {
    return net_add_multi_command(net, command, (const uchar *)arg, length);
  }

  if (ma_net_write_command(net,(uchar) command,arg,
			length ? length : (ulong) strlen(arg), 0))
  {
    if (net->last_errno == ER_NET_PACKET_TOO_LARGE)
    {
      my_set_error(mysql, CR_NET_PACKET_TOO_LARGE, SQLSTATE_UNKNOWN, 0);
      goto end;
    }
    end_server(mysql);
    if (mariadb_reconnect(mysql))
      goto end;
    if (ma_net_write_command(net,(uchar) command,arg,
			  length ? length : (ulong) strlen(arg), 0))
    {
      my_set_error(mysql, CR_SERVER_GONE_ERROR, SQLSTATE_UNKNOWN, 0);
      goto end;
    }
  }
  result=0;

  if (net->extension->multi_status > COM_MULTI_OFF)
    skipp_check= 1;

  if (!skipp_check)
  {
    result= ((mysql->packet_length=ma_net_safe_read(mysql)) == packet_error ?
	     1 : 0);
  }
 end:
  return(result);
}