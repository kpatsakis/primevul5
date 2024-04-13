  Item_outer_ref(THD *thd, Name_resolution_context *context_arg, Item **item,
                 const char *table_name_arg, const char *field_name_arg,
                 bool alias_name_used_arg):
    Item_direct_ref(thd, context_arg, item, table_name_arg, field_name_arg,
                    alias_name_used_arg),
    outer_ref(0), in_sum_func(0), found_in_select_list(1), found_in_group_by(0)
  {}