  enum Item_result result_type () const
  {
    /*
      In 10.1 Item_type_holder::result_type() returned
      Field::result_merge_type(field_type()), which returned STRING_RESULT
      for the BIT data type. In 10.2 it returns INT_RESULT, similar
      to what Field_bit::result_type() does. This should not be
      important because Item_type_holder is a limited purpose Item
      and its result_type() should not be called from outside of
      Item_type_holder. It's called only internally from decimal_int_part()
      from join_types(), to calculate "decimals" of the result data type.
      As soon as we get BIT as one of the joined types, the result field
      type cannot be numeric: it's either BIT, or VARBINARY.
    */
    return Type_handler_hybrid_real_field_type::result_type();
  }