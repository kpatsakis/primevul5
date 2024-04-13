mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level)
{
  uchar s_level[2];
  s_level[0]= (uchar)shutdown_level;
  return(ma_simple_command(mysql, COM_SHUTDOWN, (char *)s_level, 1, 0, 0));
}