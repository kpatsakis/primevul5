mysql_read_query_result(MYSQL *mysql)
{
  return test(mysql->methods->db_read_query_result(mysql)) ? 1 : 0;
}