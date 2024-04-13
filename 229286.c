void mysql_close_slow_part(MYSQL *mysql)
{
  if (mysql->net.pvio)
  {
    free_old_query(mysql);
    mysql->status=MYSQL_STATUS_READY; /* Force command */
    mysql->options.reconnect=0;
    if (mysql->net.pvio && mysql->net.buff)
      ma_simple_command(mysql, COM_QUIT,NullS,0,1,0);
    end_server(mysql);
  }
}