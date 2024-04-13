static int compare_columns(MARIA_COLUMNDEF **a_ptr, MARIA_COLUMNDEF **b_ptr)
{
  MARIA_COLUMNDEF *a= *a_ptr, *b= *b_ptr;
  enum en_fieldtype a_type, b_type;

  a_type= (a->type == FIELD_CHECK) ? FIELD_NORMAL : a->type;
  b_type= (b->type == FIELD_CHECK) ? FIELD_NORMAL : b->type;

  if (a_type == FIELD_NORMAL && !a->null_bit)
  {
    if (b_type != FIELD_NORMAL || b->null_bit)
      return -1;
    return sign((long) a->offset - (long) b->offset);
  }
  if (b_type == FIELD_NORMAL && !b->null_bit)
    return 1;
  if (a_type == b_type)
    return sign((long) a->offset - (long) b->offset);
  if (a_type == FIELD_NORMAL)
    return -1;
  if (b_type == FIELD_NORMAL)
    return 1;
  if (a_type == FIELD_SKIP_ZERO)
    return -1;
  if (b_type == FIELD_SKIP_ZERO)
    return 1;
  if (a->type != FIELD_BLOB && b->type != FIELD_BLOB)
    if (a->length != b->length)
      return sign((long) a->length - (long) b->length);
  if (a_type == FIELD_BLOB)
    return 1;
  if (b_type == FIELD_BLOB)
    return -1;
  return sign((long) a->offset - (long) b->offset);
}