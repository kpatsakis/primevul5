const char *STDCALL mysql_info(MYSQL *mysql)
{
  return (mysql)->info;
}