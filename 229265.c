unsigned int STDCALL mysql_warning_count(MYSQL *mysql)
{
  return mysql->warning_count;
}