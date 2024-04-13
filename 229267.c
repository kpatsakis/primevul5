int STDCALL mysql_set_server_option(MYSQL *mysql,
                                    enum enum_mysql_set_option option)
{
  char buffer[2];
  int2store(buffer, (uint)option);
  return(ma_simple_command(mysql, COM_SET_OPTION, buffer, sizeof(buffer), 0, 0));
}