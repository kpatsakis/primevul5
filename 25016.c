  virtual void print(String *str, enum_query_type query_type)
  {
    decimal_value.to_string(&str_value);
    str->append(str_value);
  }