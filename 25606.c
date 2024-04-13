int JOIN::rollup_write_data(uint idx, TMP_TABLE_PARAM *tmp_table_param_arg, TABLE *table_arg)
{
  uint i;
  for (i= send_group_parts ; i-- > idx ; )
  {
    /* Get reference pointers to sum functions in place */
    copy_ref_ptr_array(ref_ptrs, rollup.ref_pointer_arrays[i]);
    if ((!having || having->val_int()))
    {
      int write_error;
      Item *item;
      List_iterator_fast<Item> it(rollup.fields[i]);
      while ((item= it++))
      {
        if (item->type() == Item::NULL_ITEM && item->is_result_field())
          item->save_in_result_field(1);
      }
      copy_sum_funcs(sum_funcs_end[i+1], sum_funcs_end[i]);
      if ((write_error= table_arg->file->ha_write_tmp_row(table_arg->record[0])))
      {
	if (create_internal_tmp_table_from_heap(thd, table_arg, 
                                                tmp_table_param_arg->start_recinfo,
                                                &tmp_table_param_arg->recinfo,
                                                write_error, 0, NULL))
	  return 1;		     
      }
    }
  }
  /* Restore ref_pointer_array */
  set_items_ref_array(current_ref_ptrs);
  return 0;
}