void in_timestamp::set(uint pos, Item *item)
{
  Timestamp_or_zero_datetime *buff= &((Timestamp_or_zero_datetime *) base)[pos];
  Timestamp_or_zero_datetime_native_null native(current_thd, item, true);
  if (native.is_null())
    *buff= Timestamp_or_zero_datetime();
  else
    *buff= Timestamp_or_zero_datetime(native);
}