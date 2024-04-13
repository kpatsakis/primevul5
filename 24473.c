bool st_select_lex::add_ftfunc_to_list(THD *thd, Item_func_match *func)
{
  return !func || ftfunc_list->push_back(func, thd->mem_root); // end of memory?
}