mysql_stat(MYSQL *mysql)
{
  if (ma_simple_command(mysql, COM_STATISTICS,0,0,0,0))
    return mysql->net.last_error;
  mysql->net.read_pos[mysql->packet_length]=0;	/* End of stat string */
  if (!mysql->net.read_pos[0])
  {
    SET_CLIENT_ERROR(mysql, CR_WRONG_HOST_INFO , SQLSTATE_UNKNOWN, 0);
    return mysql->net.last_error;
  }
  return((char*) mysql->net.read_pos);
}