  bool load_data_add_outvar(THD *thd, Load_data_param *param) const
  {
    return param->add_outvar_field(thd, field);
  }