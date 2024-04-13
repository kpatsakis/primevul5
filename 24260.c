fill_record(THD *thd, TABLE *table_arg, List<Item> &fields, List<Item> &values,
            bool ignore_errors, bool update)
{
  List_iterator_fast<Item> f(fields),v(values);
  Item *value, *fld;
  Item_field *field;
  bool save_abort_on_warning= thd->abort_on_warning;
  bool save_no_errors= thd->no_errors;
  DBUG_ENTER("fill_record");

  thd->no_errors= ignore_errors;
  /*
    Reset the table->auto_increment_field_not_null as it is valid for
    only one row.
  */
  if (fields.elements)
  {
    /*
      On INSERT or UPDATE fields are checked to be from the same table,
      thus we safely can take table from the first field.
    */
    fld= (Item_field*)f++;
    if (!(field= fld->field_for_view_update()))
    {
      my_error(ER_NONUPDATEABLE_COLUMN, MYF(0), fld->name);
      goto err;
    }
    DBUG_ASSERT(field->field->table == table_arg);
    table_arg->auto_increment_field_not_null= FALSE;
    f.rewind();
  }

  while ((fld= f++))
  {
    if (!(field= fld->field_for_view_update()))
    {
      my_error(ER_NONUPDATEABLE_COLUMN, MYF(0), fld->name);
      goto err;
    }
    value=v++;
    Field *rfield= field->field;
    TABLE* table= rfield->table;
    if (table->next_number_field &&
        rfield->field_index ==  table->next_number_field->field_index)
      table->auto_increment_field_not_null= TRUE;
    if (rfield->vcol_info &&
        !value->vcol_assignment_allowed_value() &&
        table->s->table_category != TABLE_CATEGORY_TEMPORARY)
    {
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                          ER_WARNING_NON_DEFAULT_VALUE_FOR_VIRTUAL_COLUMN,
                          ER_THD(thd, ER_WARNING_NON_DEFAULT_VALUE_FOR_VIRTUAL_COLUMN),
                          rfield->field_name, table->s->table_name.str);
    }
    if (rfield->stored_in_db() &&
        (value->save_in_field(rfield, 0)) < 0 && !ignore_errors)
    {
      my_message(ER_UNKNOWN_ERROR, ER_THD(thd, ER_UNKNOWN_ERROR), MYF(0));
      goto err;
    }
    rfield->set_has_explicit_value();
  }

  if (update)
    table_arg->evaluate_update_default_function();
  else
    if (table_arg->default_field &&
        table_arg->update_default_fields(ignore_errors))
      goto err;

  /* Update virtual fields */
  if (table_arg->vfield &&
      table_arg->update_virtual_fields(table_arg->file, VCOL_UPDATE_FOR_WRITE))
    goto err;
  thd->abort_on_warning= save_abort_on_warning;
  thd->no_errors=        save_no_errors;
  DBUG_RETURN(thd->is_error());
err:
  DBUG_PRINT("error",("got error"));
  thd->abort_on_warning= save_abort_on_warning;
  thd->no_errors=        save_no_errors;
  if (fields.elements)
    table_arg->auto_increment_field_not_null= FALSE;
  DBUG_RETURN(TRUE);
}