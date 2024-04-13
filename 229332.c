unsigned long STDCALL mysql_get_server_version(MYSQL *mysql)
{
  return (unsigned long)mariadb_server_version_id(mysql);
}