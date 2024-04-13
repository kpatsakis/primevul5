mysql_list_tables(MYSQL *mysql, const char *wild)
{
  char buff[255];

  snprintf(buff, 255, "SHOW TABLES LIKE '%s'", wild ? wild : "%");
  if (mysql_query(mysql,buff))
    return(0);
  return (mysql_store_result(mysql));
}