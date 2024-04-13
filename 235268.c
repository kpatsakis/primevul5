  bool load_data_set_value(THD *thd, const char *pos, uint length,
                           const Load_data_param *param)
  {
    field->load_data_set_value(pos, length, param->charset());
    return false;
  }