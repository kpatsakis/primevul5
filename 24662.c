  Item_insert_value(THD *thd, Name_resolution_context *context_arg, Item *a)
    :Item_field(thd, context_arg, (const char *)NULL, (const char *)NULL,
                &null_clex_str),
     arg(a) {}