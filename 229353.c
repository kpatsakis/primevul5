my_bool STDCALL mariadb_get_info(MYSQL *mysql, enum mariadb_value value, void *arg)
{
  return mariadb_get_infov(mysql, value, arg);
}