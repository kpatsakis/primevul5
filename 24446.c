bool st_select_lex::add_gorder_to_list(THD *thd, Item *item, bool asc)
{
  return add_to_list(thd, gorder_list, item, asc);
}