  longlong val_int()
  {
    // following assert is redundant, because fixed=1 assigned in constructor
    DBUG_ASSERT(fixed == 1);
    return longlong_from_hex_hybrid(str_value.ptr(), str_value.length());
  }