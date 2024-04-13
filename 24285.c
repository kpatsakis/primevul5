find_field_in_table(THD *thd, TABLE *table, const char *name, uint length,
                    bool allow_rowid, uint *cached_field_index_ptr)
{
  Field **field_ptr, *field;
  uint cached_field_index= *cached_field_index_ptr;
  DBUG_ENTER("find_field_in_table");
  DBUG_PRINT("enter", ("table: '%s', field name: '%s'", table->alias.c_ptr(),
                       name));

  /* We assume here that table->field < NO_CACHED_FIELD_INDEX = UINT_MAX */
  if (cached_field_index < table->s->fields &&
      !my_strcasecmp(system_charset_info,
                     table->field[cached_field_index]->field_name, name))
    field_ptr= table->field + cached_field_index;
  else if (table->s->name_hash.records)
  {
    field_ptr= (Field**) my_hash_search(&table->s->name_hash, (uchar*) name,
                                        length);
    if (field_ptr)
    {
      /*
        field_ptr points to field in TABLE_SHARE. Convert it to the matching
        field in table
      */
      field_ptr= (table->field + (field_ptr - table->s->field));
    }
  }
  else
  {
    if (!(field_ptr= table->field))
      DBUG_RETURN((Field *)0);
    for (; *field_ptr; ++field_ptr)
      if (!my_strcasecmp(system_charset_info, (*field_ptr)->field_name, name))
        break;
  }

  if (field_ptr && *field_ptr)
  {
    *cached_field_index_ptr= (uint)(field_ptr - table->field);
    field= *field_ptr;
  }
  else
  {
    if (!allow_rowid ||
        my_strcasecmp(system_charset_info, name, "_rowid") ||
        table->s->rowid_field_offset == 0)
      DBUG_RETURN((Field*) 0);
    field= table->field[table->s->rowid_field_offset-1];
  }

  update_field_dependencies(thd, field, table);

  DBUG_RETURN(field);
}