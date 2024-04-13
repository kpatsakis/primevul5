mysql_refresh(MYSQL *mysql,uint options)
{
  uchar bits[1];
  bits[0]= (uchar) options;
  return(ma_simple_command(mysql, COM_REFRESH,(char*) bits,1,0,0));
}