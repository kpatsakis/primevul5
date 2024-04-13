bool Item_equal::contains(Field *field)
{
  Item_equal_fields_iterator it(*this);
  while (it++)
  {
    if (field->eq(it.get_curr_field()))
        return 1;
  }
  return 0;
}