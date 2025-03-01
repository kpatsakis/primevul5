fill_record(THD *thd, TABLE *table, Field **ptr, List<Item> &values,
            bool ignore_errors, bool use_value)
{
  List_iterator_fast<Item> v(values);
  List<TABLE> tbl_list;
  Item *value;
  Field *field;
  bool abort_on_warning_saved= thd->abort_on_warning;
  uint autoinc_index= table->next_number_field
                        ? table->next_number_field->field_index
                        : ~0U;
  DBUG_ENTER("fill_record");

  if (!*ptr)
  {
    /* No fields to update, quite strange!*/
    DBUG_RETURN(0);
  }

  /*
    On INSERT or UPDATE fields are checked to be from the same table,
    thus we safely can take table from the first field.
  */
  DBUG_ASSERT((*ptr)->table == table);

  /*
    Reset the table->auto_increment_field_not_null as it is valid for
    only one row.
  */
  table->auto_increment_field_not_null= FALSE;
  while ((field = *ptr++) && ! thd->is_error())
  {
    /* Ensure that all fields are from the same table */
    DBUG_ASSERT(field->table == table);

    value=v++;
    if (field->field_index == autoinc_index)
      table->auto_increment_field_not_null= TRUE;
    if (field->vcol_info &&
        !value->vcol_assignment_allowed_value() &&
        table->s->table_category != TABLE_CATEGORY_TEMPORARY)
    {
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                          ER_WARNING_NON_DEFAULT_VALUE_FOR_VIRTUAL_COLUMN,
                          ER_THD(thd, ER_WARNING_NON_DEFAULT_VALUE_FOR_VIRTUAL_COLUMN),
                          field->field_name, table->s->table_name.str);
    }

    if (use_value)
      value->save_val(field);
    else
      if (value->save_in_field(field, 0) < 0)
        goto err;
    field->set_has_explicit_value();
  }
  /* Update virtual fields */
  thd->abort_on_warning= FALSE;
  if (table->vfield &&
      table->update_virtual_fields(table->file, VCOL_UPDATE_FOR_WRITE))
    goto err;
  thd->abort_on_warning= abort_on_warning_saved;
  DBUG_RETURN(thd->is_error());

err:
  thd->abort_on_warning= abort_on_warning_saved;
  table->auto_increment_field_not_null= FALSE;
  DBUG_RETURN(TRUE);
}