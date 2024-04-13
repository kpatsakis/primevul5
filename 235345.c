  virtual bool setup(THD *thd, Item *item)
  {
    example= item;
    Type_std_attributes::set(item);
    if (item->type() == FIELD_ITEM)
      cached_field= ((Item_field *)item)->field;
    return 0;
  };