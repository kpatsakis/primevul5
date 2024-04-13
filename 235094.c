  Item_direct_ref_to_ident(THD *thd, Item_ident *item):
    Item_direct_ref(thd, item->context, (Item**)&item, item->table_name,
                    item->field_name, FALSE)
  {
    ident= item;
    ref= (Item**)&ident;
  }