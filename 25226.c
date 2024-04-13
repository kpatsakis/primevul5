  virtual bool fix_fields(THD *, Item **)
  {
    /*
      This should not normally be called, because usually before
      fix_fields() we check is_fixed() to be false.
      But historically we allow fix_fields() to be called for Items
      who return basic_const_item()==true.
    */
    DBUG_ASSERT(is_fixed());
    DBUG_ASSERT(basic_const_item());
    return false;
  }