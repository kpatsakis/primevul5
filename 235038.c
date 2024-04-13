  bool eq_def(const Field *field) 
  { 
    return cached_field ? cached_field->eq_def (field) : FALSE;
  }