my_bool STDCALL mariadb_connection(MYSQL *mysql)
{
  return (strstr(mysql->server_version, "MariaDB") ||
          strstr(mysql->server_version, "-maria-"));
}