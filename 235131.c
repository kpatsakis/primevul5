  void share_name_with(Item *item)
  {
    name= item->name;
    name_length= item->name_length;
    is_autogenerated_name= item->is_autogenerated_name;
  }