const char * STDCALL mysql_sqlstate(MYSQL *mysql)
{
  return mysql->net.sqlstate;
}