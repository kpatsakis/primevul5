  Item_default_value(THD *thd, Name_resolution_context *context_arg, Item *a,
                     bool vcol_assignment_arg)
    :Item_field(thd, context_arg, (const char *)NULL, (const char *)NULL,
                &null_clex_str), vcol_assignment_ok(vcol_assignment_arg),
     arg(a), cached_field(NULL) {}