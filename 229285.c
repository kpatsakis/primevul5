mysql_data_seek(MYSQL_RES *result, unsigned long long row)
{
  MYSQL_ROWS	*tmp=0;
  if (result->data)
    for (tmp=result->data->data; row-- && tmp ; tmp = tmp->next) ;
  result->current_row=0;
  result->data_cursor = tmp;
}