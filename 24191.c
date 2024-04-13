static bool not_null_fields_have_null_values(TABLE *table)
{
  Field **orig_field= table->field;
  Field **filled_field= table->field_to_fill();

  if (filled_field != orig_field)
  {
    THD *thd=table->in_use;
    for (uint i=0; i < table->s->fields; i++)
    {
      Field *of= orig_field[i];
      Field *ff= filled_field[i];
      if (ff != of)
      {
        // copy after-update flags to of, copy before-update flags to ff
        swap_variables(uint32, of->flags, ff->flags);
        if (ff->is_real_null())
        {
          ff->set_notnull(); // for next row WHERE condition in UPDATE
          if (convert_null_to_field_value_or_error(of) || thd->is_error())
            return true;
        }
      }
    }
  }

  return false;
}