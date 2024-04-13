  Item_ref_null_helper(THD *thd, Name_resolution_context *context_arg,
                       Item_in_subselect* master, Item **item,
		       const char *table_name_arg, const char *field_name_arg):
    Item_ref(thd, context_arg, item, table_name_arg, field_name_arg),
    owner(master) {}