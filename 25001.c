  Item_direct_ref(THD *thd, Name_resolution_context *context_arg, Item **item,
                  const char *table_name_arg,
                  const LEX_CSTRING *field_name_arg,
                  bool alias_name_used_arg= FALSE):
    Item_ref(thd, context_arg, item, table_name_arg,
             field_name_arg, alias_name_used_arg)
  {}