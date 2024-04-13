  Item_default_value(THD *thd, Name_resolution_context *context_arg, Field *a)
    :Item_field(thd, context_arg, (const char *)NULL, (const char *)NULL,
                (const char *)NULL),
    arg(NULL),cached_field(NULL) {}