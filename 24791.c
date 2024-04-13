bool Item_func_between::fix_length_and_dec_temporal(THD *thd)
{
  if (!thd->lex->is_ps_or_view_context_analysis())
  {
    for (uint i= 0; i < 3; i ++)
    {
      if (args[i]->const_item() &&
          args[i]->type_handler_for_comparison() != m_comparator.type_handler())
      {
        Item_cache *cache= m_comparator.type_handler()->Item_get_cache(thd, args[i]);
        if (!cache || cache->setup(thd, args[i]))
          return true;
        thd->change_item_tree(&args[i], cache);
      }
    }
  }
  return false;
}