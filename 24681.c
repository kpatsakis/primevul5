  Item_type_holder(THD *thd,
                   Item *item,
                   const Type_handler *handler,
                   const Type_all_attributes *attr,
                   bool maybe_null_arg)
   :Item(thd),
    Type_handler_hybrid_field_type(handler),
    Type_geometry_attributes(handler, attr),
    enum_set_typelib(attr->get_typelib())
  {
    name= item->name;
    Type_std_attributes::set(*attr);
    maybe_null= maybe_null_arg;
  }