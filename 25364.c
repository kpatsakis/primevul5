  bool val_native_from_field(Field *field, Native *to)
  {
    if ((null_value= field->is_null()))
      return true;
    return (null_value= field->val_native(to));
  }