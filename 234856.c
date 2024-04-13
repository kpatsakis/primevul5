  Item_direct_ref(THD *thd, TABLE_LIST *view_arg, Item **item,
                  const char *field_name_arg,
                  bool alias_name_used_arg= FALSE):
    Item_ref(thd, view_arg, item, field_name_arg,
             alias_name_used_arg)
  {}