  void fetch_value_from(Item *new_item)
  {
    Item *save= item;
    item= new_item;
    cmp();
    item= save;
  }