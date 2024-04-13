void cmp_item_timestamp::store_value(Item *item)
{
  item->val_native_with_conversion(current_thd, &m_native,
                                   &type_handler_timestamp2);
  m_null_value= item->null_value;
}