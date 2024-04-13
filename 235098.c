  Item_cache(THD *thd):
    Item_basic_constant(thd),
    Type_handler_hybrid_field_type(MYSQL_TYPE_STRING),
    example(0), cached_field(0),
    value_cached(0)
  {
    fixed= 1;
    maybe_null= 1;
    null_value= 1;
    null_value_inside= true;
  }