  bool fix_fields(THD *thd, Item **it)
  {
    DBUG_ASSERT(ident->type() == FIELD_ITEM || ident->type() == REF_ITEM);
    if (ident->fix_fields_if_needed_for_scalar(thd, ref))
      return TRUE;
    set_properties();
    return FALSE;
  }