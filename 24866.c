void Item_cond::fix_after_pullout(st_select_lex *new_parent, Item **ref,
                                  bool merge)
{
  List_iterator<Item> li(list);
  Item *item;

  used_tables_and_const_cache_init();

  and_tables_cache= ~(table_map) 0; // Here and below we do as fix_fields does
  not_null_tables_cache= 0;

  while ((item=li++))
  {
    table_map tmp_table_map;
    item->fix_after_pullout(new_parent, li.ref(), merge);
    item= *li.ref();
    used_tables_and_const_cache_join(item);

    if (item->const_item())
      and_tables_cache= (table_map) 0;
    else
    {
      tmp_table_map= item->not_null_tables();
      not_null_tables_cache|= tmp_table_map;
      and_tables_cache&= tmp_table_map;
      const_item_cache= FALSE;
    }  
  }
}