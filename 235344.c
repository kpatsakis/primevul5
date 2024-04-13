  bool store(struct st_value *value, ulonglong fuzzydate)
  {
    switch (cmp_type()) {
    case INT_RESULT:
    {
      value->m_type= unsigned_flag ? DYN_COL_UINT : DYN_COL_INT;
      value->value.m_longlong= val_int();
      break;
    }
    case REAL_RESULT:
    {
      value->m_type= DYN_COL_DOUBLE;
      value->value.m_double= val_real();
      break;
    }
    case DECIMAL_RESULT:
    {
      value->m_type= DYN_COL_DECIMAL;
      my_decimal *dec= val_decimal(&value->m_decimal);
      if (dec != &value->m_decimal && !null_value)
        my_decimal2decimal(dec, &value->m_decimal);
      break;
    }
    case STRING_RESULT:
    {
      value->m_type= DYN_COL_STRING;
      String *str= val_str(&value->m_string);
      if (str != &value->m_string && !null_value)
        value->m_string.set(str->ptr(), str->length(), str->charset());
      break;
    }
    case TIME_RESULT:
    {
      value->m_type= DYN_COL_DATETIME;
      get_date(&value->value.m_time, fuzzydate);
      break;
    }
    case ROW_RESULT:
      DBUG_ASSERT(false);
      null_value= true;
      break;
    }
    if (null_value)
    {
      value->m_type= DYN_COL_NULL;
      return true;
    }
    return false;
  }