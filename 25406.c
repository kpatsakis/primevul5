static int cmp_timestamp(void *cmp_arg,
                         Timestamp_or_zero_datetime *a,
                         Timestamp_or_zero_datetime *b)
{
  return a->cmp(*b);
}