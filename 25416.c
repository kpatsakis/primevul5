bool Item_in_optimizer::fix_fields(THD *thd, Item **ref)
{
  DBUG_ASSERT(fixed == 0);
  Item_subselect *sub= 0;
  uint col;

  /*
     MAX/MIN optimization can convert the subquery into
     expr + Item_singlerow_subselect
   */
  if (args[1]->type() == Item::SUBSELECT_ITEM)
    sub= (Item_subselect *)args[1];

  if (fix_left(thd))
    return TRUE;
  if (args[0]->maybe_null)
    maybe_null=1;

  if (args[1]->fix_fields_if_needed(thd, args + 1))
    return TRUE;
  if (!invisible_mode() &&
      ((sub && ((col= args[0]->cols()) != sub->engine->cols())) ||
       (!sub && (args[1]->cols() != (col= 1)))))
  {
    my_error(ER_OPERAND_COLUMNS, MYF(0), col);
    return TRUE;
  }
  if (args[1]->maybe_null)
    maybe_null=1;
  m_with_subquery= true;
  join_with_sum_func(args[1]);
  with_window_func= args[0]->with_window_func;
  // The subquery cannot have window functions aggregated in this select
  DBUG_ASSERT(!args[1]->with_window_func);
  with_field= with_field || args[1]->with_field;
  with_param= args[0]->with_param || args[1]->with_param; 
  used_tables_and_const_cache_join(args[1]);
  fixed= 1;
  return FALSE;
}