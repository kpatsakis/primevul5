  longlong val_temporal_packed(const Item *other)
  {
    return val_temporal_packed(field_type_for_temporal_comparison(other));
  }