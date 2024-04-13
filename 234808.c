pick_table_access_method(JOIN_TAB *tab)
{
  switch (tab->type) 
  {
  case JT_REF:
    tab->read_first_record= join_read_always_key;
    tab->read_record.read_record_func= join_read_next_same;
    break;

  case JT_REF_OR_NULL:
    tab->read_first_record= join_read_always_key_or_null;
    tab->read_record.read_record_func= join_read_next_same_or_null;
    break;

  case JT_CONST:
    tab->read_first_record= join_read_const;
    tab->read_record.read_record_func= join_no_more_records;
    break;

  case JT_EQ_REF:
    tab->read_first_record= join_read_key;
    tab->read_record.read_record_func= join_no_more_records;
    break;

  case JT_FT:
    tab->read_first_record= join_ft_read_first;
    tab->read_record.read_record_func= join_ft_read_next;
    break;

  case JT_SYSTEM:
    tab->read_first_record= join_read_system;
    tab->read_record.read_record_func= join_no_more_records;
    break;

  /* keep gcc happy */  
  default:
    break;  
  }
}