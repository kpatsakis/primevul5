  Longlong_null to_longlong_null()
  {
    longlong nr= val_int();
    /*
      C++ does not guarantee the order of parameter evaluation,
      so to make sure "null_value" is passed to the constructor
      after the val_int() call, val_int() is caled on a separate line.
    */
    return Longlong_null(nr, null_value);
  }