  virtual COND *build_equal_items(THD *thd, COND_EQUAL *inheited,
                                  bool link_item_fields,
                                  COND_EQUAL **cond_equal_ref)
  {
    update_used_tables();
    DBUG_ASSERT(!cond_equal_ref || !cond_equal_ref[0]);
    return this;
  }