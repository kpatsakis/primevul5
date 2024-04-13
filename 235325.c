  virtual void update_null_value ()
  {
    switch (cmp_type()) {
    case INT_RESULT:
      (void) val_int();
      break;
    case REAL_RESULT:
      (void) val_real();
      break;
    case DECIMAL_RESULT:
      {
        my_decimal tmp;
        (void) val_decimal(&tmp);
      }
      break;
    case TIME_RESULT:
      {
        MYSQL_TIME ltime;
        (void) get_temporal_with_sql_mode(&ltime);
      }
      break;
    case STRING_RESULT:
      {
        StringBuffer<MAX_FIELD_WIDTH> tmp;
        (void) val_str(&tmp);
      }
      break;
    case ROW_RESULT:
      DBUG_ASSERT(0);
      null_value= true;
    }
  }