int STDCALL mysql_get_option(MYSQL *mysql, enum mysql_option option, void *arg)
{
  return mysql_get_optionv(mysql, option, arg);
}