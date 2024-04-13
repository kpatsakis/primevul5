bool st_select_lex::add_index_hint (THD *thd, char *str, uint length)
{
  return index_hints->push_front(new (thd->mem_root) 
                                 Index_hint(current_index_hint_type,
                                            current_index_hint_clause,
                                            str, length), thd->mem_root);
}