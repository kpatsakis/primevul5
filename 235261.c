  void fix_after_pullout(st_select_lex *new_parent, Item **refptr, bool merge)
  {
    orig_item->fix_after_pullout(new_parent, &orig_item, merge);
  }