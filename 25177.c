  bool save_in_param(THD *thd, Item_param *param)
  {
    // It should not be possible to have "EXECUTE .. USING DEFAULT(a)"
    DBUG_ASSERT(0);
    param->set_default();
    return false;
  }