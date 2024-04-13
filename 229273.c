mysql_send_query(MYSQL* mysql, const char* query, unsigned long length)
{
  return ma_simple_command(mysql, COM_QUERY, query, length, 1,0);
}