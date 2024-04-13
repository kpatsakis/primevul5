void Item_func_if::fix_after_pullout(st_select_lex *new_parent,
                                     Item **ref, bool merge)
{
  /* This will re-calculate attributes of the arguments */
  Item_func::fix_after_pullout(new_parent, ref, merge);
  /* Then, re-calculate not_null_tables_cache according to our special rules */
  eval_not_null_tables(NULL);
}