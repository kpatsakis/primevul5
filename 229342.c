my_bool STDCALL mysql_rollback(MYSQL *mysql)
{
  return((my_bool)mysql_real_query(mysql, "ROLLBACK", (unsigned long)strlen("ROLLBACK")));
}