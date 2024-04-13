static COND *build_equal_items(JOIN *join, COND *cond,
                               COND_EQUAL *inherited,
                               List<TABLE_LIST> *join_list,
                               bool ignore_on_conds,
                               COND_EQUAL **cond_equal_ref,
                               bool link_equal_fields)
{
  THD *thd= join->thd;

  *cond_equal_ref= NULL;

  if (cond) 
  {
    cond= cond->build_equal_items(thd, inherited, link_equal_fields,
                                  cond_equal_ref);
    if (*cond_equal_ref)
    {
      (*cond_equal_ref)->upper_levels= inherited;
      inherited= *cond_equal_ref;
    }
  }

  if (join_list && !ignore_on_conds)
  {
    TABLE_LIST *table;
    List_iterator<TABLE_LIST> li(*join_list);

    while ((table= li++))
    {
      if (table->on_expr)
      {
        List<TABLE_LIST> *nested_join_list= table->nested_join ?
          &table->nested_join->join_list : NULL;
        /*
          We can modify table->on_expr because its old value will
          be restored before re-execution of PS/SP.
        */
        table->on_expr= build_equal_items(join, table->on_expr, inherited,
                                          nested_join_list, ignore_on_conds,
                                          &table->cond_equal);
      }
    }
  }

  return cond;
}    