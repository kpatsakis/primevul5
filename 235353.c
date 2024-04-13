  enum_field_types field_type_for_temporal_comparison(const Item *other) const
  {
    if (cmp_type() == TIME_RESULT)
    {
      if (other->cmp_type() == TIME_RESULT)
        return Field::field_type_merge(field_type(), other->field_type());
      else
        return field_type();
    }
    else
    {
      if (other->cmp_type() == TIME_RESULT)
        return other->field_type();
      DBUG_ASSERT(0); // Two non-temporal data types, we should not get to here
      return MYSQL_TYPE_DATETIME;
    }
  }