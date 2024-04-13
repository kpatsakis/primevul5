mysql_query(MYSQL *mysql, const char *query)
{
  return mysql_real_query(mysql,query, (unsigned long) strlen(query));
}