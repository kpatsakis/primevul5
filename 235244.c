  bool fix_fields(THD *thd, Item **it)
  {
    DBUG_ASSERT(ident->type() == FIELD_ITEM || ident->type() == REF_ITEM);
    if ((!ident->fixed && ident->fix_fields(thd, ref)) ||
        ident->check_cols(1))
      return TRUE;
    set_properties();
    return FALSE;
  }