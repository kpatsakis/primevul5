uchar *in_time::get_value(Item *item)
{
  tmp.val= item->val_time_packed(current_thd);
  if (item->null_value)
    return 0;
  tmp.unsigned_flag= 1L;
  return (uchar*) &tmp;
}