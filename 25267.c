Item_cond::eval_not_null_tables(void *opt_arg)
{
  Item *item;
  bool is_and_cond= functype() == Item_func::COND_AND_FUNC;
  List_iterator<Item> li(list);
  not_null_tables_cache= (table_map) 0;
  and_tables_cache= ~(table_map) 0;
  while ((item=li++))
  {
    table_map tmp_table_map;
    if (item->const_item() && !item->with_param &&
        !item->is_expensive() && !cond_has_datetime_is_null(item))
    {
      if (item->eval_const_cond() == is_and_cond && top_level())
      {
        /* 
          a. This is "... AND true_cond AND ..."
          In this case, true_cond  has no effect on cond_and->not_null_tables()
          b. This is "... OR false_cond/null cond OR ..." 
          In this case, false_cond has no effect on cond_or->not_null_tables()
        */
      }
      else
      {
        /* 
          a. This is "... AND false_cond/null_cond AND ..."
          The whole condition is FALSE/UNKNOWN.
          b. This is  "... OR const_cond OR ..."
          In this case, cond_or->not_null_tables()=0, because the condition
          const_cond might evaluate to true (regardless of whether some tables
          were NULL-complemented).
        */
        not_null_tables_cache= (table_map) 0;
        and_tables_cache= (table_map) 0;
      }
    }
    else
    {
      tmp_table_map= item->not_null_tables();
      not_null_tables_cache|= tmp_table_map;
      and_tables_cache&= tmp_table_map;
    }
  }
  return 0;
}