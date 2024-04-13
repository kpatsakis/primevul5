  enum_field_types odbc_temporal_literal_type(const LEX_STRING *type_str) const
  {
    /*
      If string is a reasonably short pure ASCII string literal,
      try to parse known ODBC style date, time or timestamp literals,
      e.g:
      SELECT {d'2001-01-01'};
      SELECT {t'10:20:30'};
      SELECT {ts'2001-01-01 10:20:30'};
    */
    if (collation.repertoire == MY_REPERTOIRE_ASCII &&
        str_value.length() < MAX_DATE_STRING_REP_LENGTH * 4)
    {
      if (type_str->length == 1)
      {
        if (type_str->str[0] == 'd')  /* {d'2001-01-01'} */
          return MYSQL_TYPE_DATE;
        else if (type_str->str[0] == 't') /* {t'10:20:30'} */
          return MYSQL_TYPE_TIME;
      }
      else if (type_str->length == 2) /* {ts'2001-01-01 10:20:30'} */
      {
        if (type_str->str[0] == 't' && type_str->str[1] == 's')
          return MYSQL_TYPE_DATETIME;
      }
    }
    return MYSQL_TYPE_STRING; // Not a temporal literal
  }