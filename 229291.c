mysql_store_result(MYSQL *mysql)
{
  MYSQL_RES *result;

  if (!mysql->fields)
    return(0);
  if (mysql->status != MYSQL_STATUS_GET_RESULT)
  {
    SET_CLIENT_ERROR(mysql, CR_COMMANDS_OUT_OF_SYNC, SQLSTATE_UNKNOWN, 0);
    return(0);
  }
  mysql->status=MYSQL_STATUS_READY;		/* server is ready */
  if (!(result=(MYSQL_RES*) calloc(1, sizeof(MYSQL_RES)+
				       sizeof(ulong)*mysql->field_count)))
  {
    SET_CLIENT_ERROR(mysql, CR_OUT_OF_MEMORY, SQLSTATE_UNKNOWN, 0);
    return(0);
  }
  result->eof=1;				/* Marker for buffered */
  result->lengths=(ulong*) (result+1);
  if (!(result->data=mysql->methods->db_read_rows(mysql,mysql->fields,mysql->field_count)))
  {
    free(result);
    return(0);
  }
  mysql->affected_rows= result->row_count= result->data->rows;
  result->data_cursor=	result->data->data;
  result->fields=	mysql->fields;
  result->field_alloc=	mysql->field_alloc;
  result->field_count=	mysql->field_count;
  result->current_field=0;
  result->current_row=0;			/* Must do a fetch first */
  mysql->fields=0;				/* fields is now in result */
  return(result);				/* Data fetched */
}