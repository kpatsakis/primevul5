int print_explain_message_line(select_result_sink *result, 
                               uint8 options, bool is_analyze,
                               uint select_number,
                               const char *select_type,
                               ha_rows *rows,
                               const char *message)
{
  THD *thd= result->thd;
  MEM_ROOT *mem_root= thd->mem_root;
  Item *item_null= new (mem_root) Item_null(thd);
  List<Item> item_list;

  item_list.push_back(new (mem_root) Item_int(thd, (int32) select_number),
                      mem_root);
  item_list.push_back(new (mem_root) Item_string_sys(thd, select_type),
                      mem_root);
  /* `table` */
  item_list.push_back(item_null, mem_root);
  
  /* `partitions` */
  if (options & DESCRIBE_PARTITIONS)
    item_list.push_back(item_null, mem_root);
  
  /* type, possible_keys, key, key_len, ref */
  for (uint i=0 ; i < 5; i++)
    item_list.push_back(item_null, mem_root);

  /* `rows` */
  if (rows)
  {
    item_list.push_back(new (mem_root) Item_int(thd, *rows,
                                     MY_INT64_NUM_DECIMAL_DIGITS),
                        mem_root);
  }
  else
    item_list.push_back(item_null, mem_root);

  /* `r_rows` */
  if (is_analyze)
    item_list.push_back(item_null, mem_root);

  /* `filtered` */
  if (is_analyze || options & DESCRIBE_EXTENDED)
    item_list.push_back(item_null, mem_root);
  
  /* `r_filtered` */
  if (is_analyze)
    item_list.push_back(item_null, mem_root);

  /* `Extra` */
  if (message)
    item_list.push_back(new (mem_root) Item_string_sys(thd, message),
                        mem_root);
  else
    item_list.push_back(item_null, mem_root);

  if (result->send_data(item_list))
    return 1;
  return 0;
}