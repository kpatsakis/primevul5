  bool set_fields_as_dependent_processor(void *arg)
  {
    if (!(used_tables() & OUTER_REF_TABLE_BIT))
    {
      depended_from= (st_select_lex *) arg;
      item_equal= NULL;
    }
    return 0;
  }