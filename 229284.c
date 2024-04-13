int STDCALL mysql_next_result(MYSQL *mysql)
{

  /* make sure communication is not blocking */
  if (mysql->status != MYSQL_STATUS_READY)
  {
    SET_CLIENT_ERROR(mysql, CR_COMMANDS_OUT_OF_SYNC, SQLSTATE_UNKNOWN, 0);
    return(1);
  }

  /* clear error, and mysql status variables */
  CLEAR_CLIENT_ERROR(mysql);
  mysql->affected_rows = (ulonglong) ~0;

  if (mysql->server_status & SERVER_MORE_RESULTS_EXIST)
  {
     return(mysql->methods->db_read_query_result(mysql));
  }

  return(-1);
}