  Field *make_conversion_table_field(TABLE *table,
                                     uint metadata, const Field *target) const
  {
    DBUG_ASSERT(0); // Should not be called in Item context
    return NULL;
  }