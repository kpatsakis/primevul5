const char * STDCALL mysql_error(MYSQL *mysql)
{
  return mysql ? (mysql)->net.last_error : (char *)"";
}