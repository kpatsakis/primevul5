  String *val_str_from_item(Item *item, String *str)
  {
    DBUG_ASSERT(is_fixed());
    String *res= item->val_str(str);
    if (res)
      res->set_charset(collation.collation);
    if ((null_value= item->null_value))
      res= NULL;
    return res;
  }