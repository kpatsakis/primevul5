  Item_ref(THD *thd, Item_ref *item)
    :Item_ident(thd, item), set_properties_only(0), ref(item->ref) {}