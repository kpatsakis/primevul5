uchar *in_timestamp::get_value(Item *item)
{
  Timestamp_or_zero_datetime_native_null native(current_thd, item, true);
  if (native.is_null())
    return 0;
  tmp= Timestamp_or_zero_datetime(native);
  return (uchar*) &tmp;
}