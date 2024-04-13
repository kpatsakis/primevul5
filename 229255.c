mysql_dump_debug_info(MYSQL *mysql)
{
  return(ma_simple_command(mysql, COM_DEBUG,0,0,0,0));
}