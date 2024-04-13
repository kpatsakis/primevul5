  longlong val_int_from_real()
  {
    DBUG_ASSERT(is_fixed());
    return Converter_double_to_longlong_with_warn(val_real(), false).result();
  }