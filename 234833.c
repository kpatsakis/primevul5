  Item_copy(THD *thd, Item *i): Item(thd)
  {
    item= i;
    null_value=maybe_null=item->maybe_null;
    Type_std_attributes::set(item);
    name=item->name;
    set_handler_by_field_type(item->field_type());
    fixed= item->fixed;
  }