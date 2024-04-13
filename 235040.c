  Item_int_with_ref(THD *thd, longlong i, Item *ref_arg, bool unsigned_arg):
    Item_int(thd, i), ref(ref_arg)
  {
    unsigned_flag= unsigned_arg;
  }