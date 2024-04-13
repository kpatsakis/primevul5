  Item_type_holder(THD *thd, Item *item)
   :Item(thd, item),
    Type_handler_hybrid_field_type(item->real_type_handler()),
    enum_set_typelib(0)
  {
    DBUG_ASSERT(item->is_fixed());
    maybe_null= item->maybe_null;
  }