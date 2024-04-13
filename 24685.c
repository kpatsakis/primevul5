  bool is_valid_limit_clause_variable_with_error() const
  {
    /*
      In case if the variable has an anchored data type, e.g.:
        DECLARE a TYPE OF t1.a;
      type_handler() is set to &type_handler_null and this
      function detects such variable as not valid in LIMIT.
    */
    if (type_handler()->is_limit_clause_valid_type())
      return true;
    my_error(ER_WRONG_SPVAR_TYPE_IN_LIMIT, MYF(0));
    return false;
  }