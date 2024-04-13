Type_handler_hybrid_field_type::aggregate_for_comparison(const char *funcname,
                                                         Item **items,
                                                         uint nitems,
                                                         bool int_uint_as_dec)
{
  uint unsigned_count= items[0]->unsigned_flag;
  /*
    Convert sub-type to super-type (e.g. DATE to DATETIME, INT to BIGINT, etc).
    Otherwise Predicant_to_list_comparator will treat sub-types of the same
    super-type as different data types and won't be able to use bisection in
    many cases.
  */
  set_handler(items[0]->type_handler()->type_handler_for_comparison());
  for (uint i= 1 ; i < nitems ; i++)
  {
    unsigned_count+= items[i]->unsigned_flag;
    if (aggregate_for_comparison(items[i]->type_handler()->
                                 type_handler_for_comparison()))
    {
      /*
        For more precise error messages if aggregation failed on the first pair
        {items[0],items[1]}, use the name of items[0]->data_handler().
        Otherwise use the name of this->type_handler(), which is already a
        result of aggregation for items[0]..items[i-1].
      */
      my_error(ER_ILLEGAL_PARAMETER_DATA_TYPES2_FOR_OPERATION, MYF(0),
               i == 1 ? items[0]->type_handler()->name().ptr() :
                        type_handler()->name().ptr(),
               items[i]->type_handler()->name().ptr(),
               funcname);
      return true;
    }
    /*
      When aggregating types of two row expressions we have to check
      that they have the same cardinality and that each component
      of the first row expression has a compatible row signature with
      the signature of the corresponding component of the second row
      expression.
    */ 
    if (cmp_type() == ROW_RESULT && cmp_row_type(items[0], items[i]))
      return true;     // error found: invalid usage of rows
  }
  /**
    If all arguments are of INT type but have different unsigned_flag values,
    switch to DECIMAL_RESULT.
  */
  if (int_uint_as_dec &&
      cmp_type() == INT_RESULT &&
      unsigned_count != nitems && unsigned_count != 0)
    set_handler(&type_handler_newdecimal);
  return 0;
}