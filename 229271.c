mysql_options4(MYSQL *mysql,enum mysql_option option, const void *arg1, const void *arg2)
{
  return mysql_optionsv(mysql, option, arg1, arg2);
}