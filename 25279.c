void in_timestamp::value_to_item(uint pos, Item *item)
{
  const Timestamp_or_zero_datetime &buff= (((Timestamp_or_zero_datetime*) base)[pos]);
  static_cast<Item_timestamp_literal*>(item)->set_value(buff);
}