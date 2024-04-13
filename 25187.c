Item** Arg_comparator::cache_converted_constant(THD *thd_arg, Item **value,
                                                Item **cache_item,
                                                const Type_handler *handler)
{
  /*
    Don't need cache if doing context analysis only.
  */
  if (!thd_arg->lex->is_ps_or_view_context_analysis() &&
      (*value)->const_item() &&
      handler->type_handler_for_comparison() !=
      (*value)->type_handler_for_comparison())
  {
    Item_cache *cache= handler->Item_get_cache(thd_arg, *value);
    cache->setup(thd_arg, *value);
    *cache_item= cache;
    return cache_item;
  }
  return value;
}