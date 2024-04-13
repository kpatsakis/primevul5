my_bool STDCALL mysql_commit(MYSQL *mysql)
{
  return((my_bool)mysql_real_query(mysql, "COMMIT", (unsigned long)strlen("COMMIT")));
}