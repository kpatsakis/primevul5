mysql_fetch_lengths(MYSQL_RES *res)
{
  ulong *lengths,*prev_length;
  char *start;
  MYSQL_ROW column,end;

  if (!(column=res->current_row))
    return 0;					/* Something is wrong */
  if (res->data)
  {
    start=0;
    prev_length=0;				/* Keep gcc happy */
    lengths=res->lengths;
    for (end=column+res->field_count+1 ; column != end ; column++,lengths++)
    {
      if (!*column)
      {
	*lengths=0;				/* Null */
	continue;
      }
      if (start)				/* Found end of prev string */
	*prev_length= (uint) (*column-start-1);
      start= *column;
      prev_length=lengths;
    }
  }
  return res->lengths;
}