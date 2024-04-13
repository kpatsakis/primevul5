  Item_ref(THD *thd, Name_resolution_context *context_arg,
           const char *db_arg, const char *table_name_arg,
           const char *field_name_arg):
    Item_ident(thd, context_arg, db_arg, table_name_arg, field_name_arg),
    set_properties_only(0), ref(0), reference_trough_name(1) {}