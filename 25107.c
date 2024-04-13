  Item_direct_view_ref(THD *thd, Name_resolution_context *context_arg,
                       Item **item,
                       const char *table_name_arg,
                       LEX_CSTRING *field_name_arg,
                       TABLE_LIST *view_arg):
    Item_direct_ref(thd, context_arg, item, table_name_arg, field_name_arg),
    item_equal(0), view(view_arg),
    null_ref_table(NULL)
  {
    if (fixed)
      set_null_ref_table();
  }