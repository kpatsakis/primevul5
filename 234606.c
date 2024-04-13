static bool compare_record(TABLE *table, Field **ptr)
{
  for (; *ptr ; ptr++)
  {
    Field *f= *ptr;
    if (f->is_null() != f->is_null(table->s->rec_buff_length) ||
        (!f->is_null() && f->cmp_offset(table->s->rec_buff_length)))
      return 1;
  }
  return 0;
}