mysql_fetch_row(MYSQL_RES *res)
{
  if (!res)
    return 0;
  if (res->handle)
  {
    if (res->handle->status != MYSQL_STATUS_USE_RESULT &&
        res->handle->status != MYSQL_STATUS_GET_RESULT)
      return 0;
  }
  if (!res->data)
  {						/* Unbufferred fetch */
    if (!res->eof && res->handle)
    {
      if (!(res->handle->methods->db_read_one_row(res->handle,res->field_count,res->row, res->lengths)))
      {
        res->row_count++;
        return(res->current_row=res->row);
      }
      res->eof=1;
      res->handle->status=MYSQL_STATUS_READY;
       /* Don't clear handle in mysql_free_results */
      res->handle=0;
    }
    return((MYSQL_ROW) NULL);
  }
  {
    MYSQL_ROW tmp;
    if (!res->data_cursor)
    {
      return(res->current_row=(MYSQL_ROW) NULL);
    }
    tmp = res->data_cursor->data;
    res->data_cursor = res->data_cursor->next;
    return(res->current_row=tmp);
  }
}