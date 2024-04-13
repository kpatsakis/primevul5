mysql_list_dbs(MYSQL *mysql, const char *wild)
{
  char buff[255];
  snprintf(buff, 255, "SHOW DATABASES LIKE '%s'", wild ? wild : "%");
  if (mysql_query(mysql,buff))
    return(0);
  return (mysql_store_result(mysql));
}