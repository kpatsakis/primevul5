join_read_record_no_init(JOIN_TAB *tab)
{
  Copy_field *save_copy, *save_copy_end;
  
  /*
    init_read_record resets all elements of tab->read_record().
    Remember things that we don't want to have reset.
  */
  save_copy=     tab->read_record.copy_field;
  save_copy_end= tab->read_record.copy_field_end;
  
  init_read_record(&tab->read_record, tab->join->thd, tab->table,
		   tab->select, tab->filesort_result, 1, 1, FALSE);

  tab->read_record.copy_field=     save_copy;
  tab->read_record.copy_field_end= save_copy_end;
  tab->read_record.read_record_func= rr_sequential_and_unpack;

  return tab->read_record.read_record();
}