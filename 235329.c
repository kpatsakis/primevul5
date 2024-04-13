  Item_outer_ref(THD *thd, Name_resolution_context *context_arg,
                 Item_field *outer_field_arg):
    Item_direct_ref(thd, context_arg, 0, outer_field_arg->table_name,
                    outer_field_arg->field_name),
    outer_ref(outer_field_arg), in_sum_func(0),
    found_in_select_list(0), found_in_group_by(0)
  {
    ref= &outer_ref;
    set_properties();
    fixed= 0;                     /* reset flag set in set_properties() */
  }