uint STDCALL mysql_field_tell(MYSQL_RES *res)
{
  return (res)->current_field;
}