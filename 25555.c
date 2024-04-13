static int item_is_unsigned(struct st_mysql_value *value)
{
  Item *item= ((st_item_value_holder*)value)->item;
  return item->unsigned_flag;
}