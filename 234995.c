  longlong val_temporal_packed(enum_field_types f_type)
  {
    return f_type == MYSQL_TYPE_TIME ? val_time_packed() :
                                       val_datetime_packed();
  }