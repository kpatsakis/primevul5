  longlong val_int()
  {
    DBUG_ASSERT(0); // never should be called
    null_value= true;
    return 0;
  }