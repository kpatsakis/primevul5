mysql_use_result(MYSQL *mysql)
{
  MYSQL_RES *result;

  if (!mysql->fields)
    return(0);
  if (mysql->status != MYSQL_STATUS_GET_RESULT)
  {
    SET_CLIENT_ERROR(mysql, CR_COMMANDS_OUT_OF_SYNC, SQLSTATE_UNKNOWN, 0);
    return(0);
  }
  if (!(result=(MYSQL_RES*) calloc(1, sizeof(*result)+
				      sizeof(ulong)*mysql->field_count)))
    return(0);
  result->lengths=(ulong*) (result+1);
  if (!(result->row=(MYSQL_ROW)
	malloc(sizeof(result->row[0])*(mysql->field_count+1))))
  {					/* Ptrs: to one row */
    free(result);
    return(0);
  }
  result->fields=	mysql->fields;
  result->field_alloc=	mysql->field_alloc;
  result->field_count=	mysql->field_count;
  result->current_field=0;
  result->handle=	mysql;
  result->current_row=	0;
  mysql->fields=0;			/* fields is now in result */
  mysql->status=MYSQL_STATUS_USE_RESULT;
  return(result);			/* Data is read to be fetched */
}