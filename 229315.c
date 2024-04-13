mysql_ping(MYSQL *mysql)
{
  int rc;
  rc= ma_simple_command(mysql, COM_PING, 0, 0, 0, 0);
  if (rc && mysql->options.reconnect)
    rc= ma_simple_command(mysql, COM_PING, 0, 0, 0, 0);
  return rc;
}