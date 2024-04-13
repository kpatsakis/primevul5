  bool cleanup_excluding_const_fields_processor(void *arg)
  { 
    Item *item= real_item();
    if (item && item->type() == FIELD_ITEM &&
        ((Item_field *) item)->field && item->const_item())
      return 0;
    return cleanup_processor(arg);
  }