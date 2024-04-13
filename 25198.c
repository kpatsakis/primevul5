  void set_typelib(TYPELIB *typelib)
  {
    // Non-field Items (e.g. hybrid functions) never have ENUM/SET types yet.
    DBUG_ASSERT(0);
  }