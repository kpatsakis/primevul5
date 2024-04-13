mysql_get_server_name(MYSQL *mysql)
{
  if (mysql->options.extension &&
      mysql->options.extension->db_driver != NULL)
    return mysql->options.extension->db_driver->name;
  return mariadb_connection(mysql) ? "MariaDB" : "MySQL";
}