  void set(const Field *field)
  {
    decimals= field->decimals();
    max_length= field->field_length;
    collation.set(field->charset());
    unsigned_flag= MY_TEST(field->flags & UNSIGNED_FLAG);
  }