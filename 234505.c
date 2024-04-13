bool Virtual_tmp_table::sp_find_field_by_name(uint *idx,
                                              const LEX_CSTRING &name) const
{
  Field *f;
  for (uint i= 0; (f= field[i]); i++)
  {
    // Use the same comparison style with sp_context::find_variable()
    if (!my_strnncoll(system_charset_info,
                      (const uchar *) f->field_name.str,
                      f->field_name.length,
                      (const uchar *) name.str, name.length))
    {
      *idx= i;
      return false;
    }
  }
  return true;
}