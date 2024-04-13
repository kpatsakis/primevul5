unsigned int STDCALL mysql_num_fields(MYSQL_RES *res)
{
  return res->field_count;
}