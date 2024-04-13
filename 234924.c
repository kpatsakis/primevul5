  void save_org_in_field(Field *field,
                         fast_field_copier data __attribute__ ((__unused__)))
  {
    if (check_null_ref())
      field->set_null();
    else
      Item_direct_ref::save_val(field);
  }