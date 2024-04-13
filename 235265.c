  bool cleanup_excluding_fields_processor(void *arg)
  {
    Item *item= real_item();
    if (item && item->type() == FIELD_ITEM &&
        ((Item_field *)item)->field)
      return 0;
    return cleanup_processor(arg);
  }