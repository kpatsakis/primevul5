  Item_copy(THD *thd, Item *i): Item(thd)
  {
    DBUG_ASSERT(i->is_fixed());
    item= i;
    null_value=maybe_null=item->maybe_null;
    Type_std_attributes::set(item);
    name= item->name;
    set_handler(item->type_handler());
  }