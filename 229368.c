my_bool STDCALL mysql_autocommit(MYSQL *mysql, my_bool mode)
{
  return((my_bool) mysql_real_query(mysql, (mode) ? "SET autocommit=1" :
                                         "SET autocommit=0", 16));
}