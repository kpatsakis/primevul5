  bool save_in_param(THD *thd, Item_param *param)
  {
    param->set_default();
    return false;
  }