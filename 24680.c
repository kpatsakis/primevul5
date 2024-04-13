bool Item_in_optimizer::fix_left(THD *thd)
{
  DBUG_ENTER("Item_in_optimizer::fix_left");
  /*
    Here we will store pointer on place of main storage of left expression.
    For usual IN (ALL/ANY) it is subquery left_expr.
    For other cases (MAX/MIN optimization, non-transformed EXISTS (10.0))
    it is args[0].
  */
  Item **ref0= args;
  if (!invisible_mode())
  {
    /*
       left_expr->fix_fields() may cause left_expr to be substituted for
       another item. (e.g. an Item_field may be changed into Item_ref). This
       transformation is undone at the end of statement execution (e.g. the
       Item_ref is deleted). However, Item_in_optimizer::args[0] may keep
       the pointer to the post-transformation item. Because of that, on the
       next execution we need to copy args[1]->left_expr again.
    */
    ref0= &(((Item_in_subselect *)args[1])->left_expr);
    args[0]= ((Item_in_subselect *)args[1])->left_expr;
  }
  if ((*ref0)->fix_fields_if_needed(thd, ref0) ||
      (!cache && !(cache= (*ref0)->get_cache(thd))))
    DBUG_RETURN(1);
  /*
    During fix_field() expression could be substituted.
    So we copy changes before use
  */
  if (args[0] != (*ref0))
    args[0]= (*ref0);
  DBUG_PRINT("info", ("actual fix fields"));

  cache->setup(thd, args[0]);
  if (cache->cols() == 1)
  {
    DBUG_ASSERT(args[0]->type() != ROW_ITEM);
    /* 
      Note: there can be cases when used_tables()==0 && !const_item(). See
      Item_sum::update_used_tables for details.
    */
    if ((used_tables_cache= args[0]->used_tables()) || !args[0]->const_item())
      cache->set_used_tables(OUTER_REF_TABLE_BIT);
    else
      cache->set_used_tables(0);
  }
  else
  {
    uint n= cache->cols();
    for (uint i= 0; i < n; i++)
    {
      /* Check that the expression (part of row) do not contain a subquery */
      if (args[0]->element_index(i)->walk(&Item::is_subquery_processor, 0, 0))
      {
        my_error(ER_NOT_SUPPORTED_YET, MYF(0),
                 "SUBQUERY in ROW in left expression of IN/ALL/ANY");
        DBUG_RETURN(1);
      }
      Item *element=args[0]->element_index(i);
      if (element->used_tables() || !element->const_item())
      {
	((Item_cache *)cache->element_index(i))->
          set_used_tables(OUTER_REF_TABLE_BIT);
        cache->set_used_tables(OUTER_REF_TABLE_BIT);
      }
      else
	((Item_cache *)cache->element_index(i))->set_used_tables(0);
    }
    used_tables_cache= args[0]->used_tables();
  }
  eval_not_null_tables(NULL);
  copy_with_sum_func(args[0]);
  with_param= args[0]->with_param || args[1]->with_param;
  with_field= args[0]->with_field;
  if ((const_item_cache= args[0]->const_item()))
  {
    cache->store(args[0]);
    cache->cache_value();
  }
  if (args[1]->is_fixed())
  {
    /* to avoid overriding is called to update left expression */
    used_tables_and_const_cache_join(args[1]);
    join_with_sum_func(args[1]);
  }
  DBUG_RETURN(0);
}