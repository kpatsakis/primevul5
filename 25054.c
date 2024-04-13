  double val_real()
  {
    DBUG_ASSERT(0); // never should be called
    null_value= true;
    return 0.0;
  }