  enum Type type() const
  {
    // Don't pretend to be a constant unless value for this item is set.
    switch (state) {
    case NO_VALUE:         return PARAM_ITEM;
    case NULL_VALUE:       return NULL_ITEM;
    case SHORT_DATA_VALUE: return CONST_ITEM;
    case LONG_DATA_VALUE:  return CONST_ITEM;
    case DEFAULT_VALUE:    return PARAM_ITEM;
    case IGNORE_VALUE:     return PARAM_ITEM;
    }
    DBUG_ASSERT(0);
    return PARAM_ITEM;
  }