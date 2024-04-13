Virtual_tmp_table::sp_find_field_by_name_or_error(uint *idx,
                                                  const LEX_CSTRING &var_name,
                                                  const LEX_CSTRING &field_name)
                                                  const
{
  if (sp_find_field_by_name(idx, field_name))
  {
    my_error(ER_ROW_VARIABLE_DOES_NOT_HAVE_FIELD, MYF(0),
             var_name.str, field_name.str);
    return true;
  }
  return false;
}