ulong STDCALL mysql_net_read_packet(MYSQL *mysql)
{
  return ma_net_safe_read(mysql);
}