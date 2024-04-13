mysql_options(MYSQL *mysql,enum mysql_option option, const void *arg)
{
  return mysql_optionsv(mysql, option, arg);
}