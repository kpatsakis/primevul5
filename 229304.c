int mthd_my_read_query_result(MYSQL *mysql)
{
  uchar *pos;
  ulong field_count;
  MYSQL_DATA *fields;
  ulong length;
  my_bool can_local_infile= (mysql->options.extension) && (mysql->extension->auto_local_infile != WAIT_FOR_QUERY);

  if (mysql->options.extension && mysql->extension->auto_local_infile == ACCEPT_FILE_REQUEST)
    mysql->extension->auto_local_infile= WAIT_FOR_QUERY;

  if ((length = ma_net_safe_read(mysql)) == packet_error)
  {
    return(1);
  }
  free_old_query(mysql);			/* Free old result */
get_info:
  pos=(uchar*) mysql->net.read_pos;
  if ((field_count= net_field_length(&pos)) == 0)
    return ma_read_ok_packet(mysql, pos, length);
  if (field_count == NULL_LENGTH)		/* LOAD DATA LOCAL INFILE */
  {
    int error=mysql_handle_local_infile(mysql, (char *)pos, can_local_infile);

    if ((length=ma_net_safe_read(mysql)) == packet_error || error)
      return(-1);
    goto get_info;				/* Get info packet */
  }
  if (!(mysql->server_status & SERVER_STATUS_AUTOCOMMIT))
    mysql->server_status|= SERVER_STATUS_IN_TRANS;

  mysql->extra_info= net_field_length_ll(&pos); /* Maybe number of rec */
  if (!(fields=mysql->methods->db_read_rows(mysql,(MYSQL_FIELD*) 0,
                                            ma_result_set_rows(mysql))))
    return(-1);
  if (!(mysql->fields=unpack_fields(mysql, fields, &mysql->field_alloc,
				    (uint) field_count, 1)))
    return(-1);
  mysql->status=MYSQL_STATUS_GET_RESULT;
  mysql->field_count=field_count;
  return(0);
}