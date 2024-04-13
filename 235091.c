  Item_return_int(THD *thd, const char *name_arg, uint length,
		  enum_field_types field_type_arg, longlong value_arg= 0):
    Item_int(thd, name_arg, value_arg, length), int_field_type(field_type_arg)
  {
    unsigned_flag=1;
  }