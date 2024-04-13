  virtual Item_basic_constant *make_string_literal_concat(THD *thd,
                                                          const LEX_CSTRING *)
  {
    DBUG_ASSERT(0);
    return this;
  }