uchar *in_datetime::get_value(Item *item)
{
  tmp.val= item->val_datetime_packed(current_thd);
  if (item->null_value)
    return 0;
  tmp.unsigned_flag= 1L;
  return (uchar*) &tmp;
}