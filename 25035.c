  bool vcol_assignment_allowed_value() const
  {
    switch (state) {
    case NULL_VALUE:
    case DEFAULT_VALUE:
    case IGNORE_VALUE:
      return true;
    case NO_VALUE:
    case SHORT_DATA_VALUE:
    case LONG_DATA_VALUE:
      break;
    }
    return false;
  }