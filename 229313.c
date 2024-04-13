mysql_list_processes(MYSQL *mysql)
{
  MYSQL_DATA *fields;
  uint field_count;
  uchar *pos;

  LINT_INIT(fields);
  if (ma_simple_command(mysql, COM_PROCESS_INFO,0,0,0,0))
    return(NULL);
  free_old_query(mysql);
  pos=(uchar*) mysql->net.read_pos;
  field_count=(uint) net_field_length(&pos);
  if (!(fields = mysql->methods->db_read_rows(mysql,(MYSQL_FIELD*) 0,7)))
    return(NULL);
  if (!(mysql->fields= unpack_fields(mysql, fields, &mysql->field_alloc,
                                     field_count, 0)))
    return(NULL);
  mysql->status=MYSQL_STATUS_GET_RESULT;
  mysql->field_count=field_count;
  return(mysql_store_result(mysql));
}