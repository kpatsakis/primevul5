  Item_fixed_hybrid(THD *thd, Item_fixed_hybrid *item)
   :Item(thd, item), fixed(item->fixed)
  { }