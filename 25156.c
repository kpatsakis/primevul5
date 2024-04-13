  const Item_const *get_item_const() const
  {
    switch (state) {
    case SHORT_DATA_VALUE:
    case LONG_DATA_VALUE:
    case NULL_VALUE:
      return this;
    case IGNORE_VALUE:
    case DEFAULT_VALUE:
    case NO_VALUE:
      break;
    }
    return NULL;
  }