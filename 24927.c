longlong Item_in_optimizer::val_int()
{
  bool tmp;
  DBUG_ASSERT(fixed == 1);
  cache->store(args[0]);
  cache->cache_value();
  DBUG_ENTER(" Item_in_optimizer::val_int");

  if (invisible_mode())
  {
    longlong res= args[1]->val_int();
    null_value= args[1]->null_value;
    DBUG_PRINT("info", ("pass trough"));
    DBUG_RETURN(res);
  }

  if (cache->null_value_inside)
  {
     DBUG_PRINT("info", ("Left NULL..."));
    /*
      We're evaluating 
      "<outer_value_list> [NOT] IN (SELECT <inner_value_list>...)" 
      where one or more of the outer values is NULL. 
    */
    if (((Item_in_subselect*)args[1])->is_top_level_item())
    {
      /*
        We're evaluating a top level item, e.g. 
	"<outer_value_list> IN (SELECT <inner_value_list>...)",
	and in this case a NULL value in the outer_value_list means
        that the result shall be NULL/FALSE (makes no difference for
        top level items). The cached value is NULL, so just return
        NULL.
      */
      null_value= 1;
    }
    else
    {
      /*
	We're evaluating an item where a NULL value in either the
        outer or inner value list does not automatically mean that we
        can return NULL/FALSE. An example of such a query is
        "<outer_value_list> NOT IN (SELECT <inner_value_list>...)" 
        The result when there is at least one NULL value is: NULL if the
        SELECT evaluated over the non-NULL values produces at least
        one row, FALSE otherwise
      */
      Item_in_subselect *item_subs=(Item_in_subselect*)args[1]; 
      bool all_left_cols_null= true;
      const uint ncols= cache->cols();

      /*
        Turn off the predicates that are based on column compares for
        which the left part is currently NULL
      */
      for (uint i= 0; i < ncols; i++)
      {
        if (cache->element_index(i)->null_value)
          item_subs->set_cond_guard_var(i, FALSE);
        else 
          all_left_cols_null= false;
      }

      if (!item_subs->is_correlated && 
          all_left_cols_null && result_for_null_param != UNKNOWN)
      {
        /* 
           This is a non-correlated subquery, all values in the outer
           value list are NULL, and we have already evaluated the
           subquery for all NULL values: Return the same result we
           did last time without evaluating the subquery.
        */
        null_value= result_for_null_param;
      } 
      else 
      {
        /* The subquery has to be evaluated */
        (void) item_subs->val_bool_result();
        if (item_subs->engine->no_rows())
          null_value= item_subs->null_value;
        else
          null_value= TRUE;
        if (all_left_cols_null)
          result_for_null_param= null_value;
      }

      /* Turn all predicates back on */
      for (uint i= 0; i < ncols; i++)
        item_subs->set_cond_guard_var(i, TRUE);
    }
    DBUG_RETURN(0);
  }
  tmp= args[1]->val_bool_result();
  null_value= args[1]->null_value;
  DBUG_RETURN(tmp);
}