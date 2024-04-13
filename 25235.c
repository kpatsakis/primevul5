void Item_in_optimizer::fix_after_pullout(st_select_lex *new_parent,
                                          Item **ref, bool merge)
{
  DBUG_ASSERT(fixed);
  /* This will re-calculate attributes of our Item_in_subselect: */
  Item_bool_func::fix_after_pullout(new_parent, ref, merge);

  /* Then, re-calculate not_null_tables_cache: */
  eval_not_null_tables(NULL);
}