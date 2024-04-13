static void update_depend_map(JOIN *join)
{
  JOIN_TAB *join_tab;
  for (join_tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITH_CONST_TABLES); 
       join_tab;
       join_tab= next_linear_tab(join, join_tab, WITH_BUSH_ROOTS))
  {
    TABLE_REF *ref= &join_tab->ref;
    table_map depend_map=0;
    Item **item=ref->items;
    uint i;
    for (i=0 ; i < ref->key_parts ; i++,item++)
      depend_map|=(*item)->used_tables();
    depend_map&= ~OUTER_REF_TABLE_BIT;
    ref->depend_map= depend_map;
    for (JOIN_TAB **tab=join->map2table;
         depend_map ;
         tab++,depend_map>>=1 )
    {
      if (depend_map & 1)
        ref->depend_map|=(*tab)->ref.depend_map;
    }
  }
}