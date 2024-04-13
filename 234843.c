  longlong val_int_from_real()
  {
    DBUG_ASSERT(fixed == 1);
    return Converter_double_to_longlong_with_warn(val_real(), false).result();
  }