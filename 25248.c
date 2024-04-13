  bool fix_fields(THD *thd, Item **ref)
  {
    DBUG_ASSERT(!fixed);
    fixed= true;
    return false;
  }