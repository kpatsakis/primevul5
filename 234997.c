  Item *next()
  {
    if (current >= base_item->cols())
      return NULL;
    return base_item->element_index(current++);
  }