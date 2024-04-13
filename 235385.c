fill_record(THD *thd, TABLE *table_arg, List<Item> &fields, List<Item> &values,
            bool ignore_errors, bool update)
{
  List_iterator_fast<Item> f(fields),v(values);
  Item *value, *fld;
  Item_field *field;
  Field *rfield;
  TABLE *table;
  bool only_unvers_fields= update && table_arg->versioned();
  bool save_abort_on_warning= thd->abort_on_warning;
  bool save_no_errors= thd->no_errors;
  DBUG_ENTER("fill_record");

  thd->no_errors= ignore_errors;
  /*
    Reset the table->auto_increment_field_not_null as it is valid for
    only one row.
  */
  if (fields.elements)
    table_arg->auto_increment_field_not_null= FALSE;

  while ((fld= f++))
  {
    if (!(field= fld->field_for_view_update()))
    {
      my_error(ER_NONUPDATEABLE_COLUMN, MYF(0), fld->name.str);
      goto err;
    }
    value=v++;
    DBUG_ASSERT(value);
    rfield= field->field;
    table= rfield->table;
    if (table->next_number_field &&
        rfield->field_index ==  table->next_number_field->field_index)
      table->auto_increment_field_not_null= TRUE;
    const bool skip_sys_field= rfield->vers_sys_field(); // TODO: && !thd->vers_modify_history() [MDEV-16546]
    if ((rfield->vcol_info || skip_sys_field) &&
        !value->vcol_assignment_allowed_value() &&
        table->s->table_category != TABLE_CATEGORY_TEMPORARY)
    {
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                          ER_WARNING_NON_DEFAULT_VALUE_FOR_GENERATED_COLUMN,
                          ER_THD(thd, ER_WARNING_NON_DEFAULT_VALUE_FOR_GENERATED_COLUMN),
                          rfield->field_name.str, table->s->table_name.str);
    }
    if (only_unvers_fields && !rfield->vers_update_unversioned())
      only_unvers_fields= false;

    if (rfield->stored_in_db())
    {
      if (!skip_sys_field &&
          unlikely(value->save_in_field(rfield, 0) < 0) && !ignore_errors)
      {
        my_message(ER_UNKNOWN_ERROR, ER_THD(thd, ER_UNKNOWN_ERROR), MYF(0));
        goto err;
      }
      /*
        In sql MODE_SIMULTANEOUS_ASSIGNMENT,
        move field pointer on value stored in record[1]
        which contains row before update (see MDEV-13417)
      */
      if (update && thd->variables.sql_mode & MODE_SIMULTANEOUS_ASSIGNMENT)
        rfield->move_field_offset((my_ptrdiff_t) (table->record[1] -
                                                  table->record[0]));
    }
    rfield->set_has_explicit_value();
  }

  if (update && thd->variables.sql_mode & MODE_SIMULTANEOUS_ASSIGNMENT)
  {
    // restore fields pointers on record[0]
    f.rewind();
    while ((fld= f++))
    {
      rfield= fld->field_for_view_update()->field;
      if (rfield->stored_in_db())
      {
        table= rfield->table;
        rfield->move_field_offset((my_ptrdiff_t) (table->record[0] -
                                                  table->record[1]));
      }
    }
  }

  if (update)
    table_arg->evaluate_update_default_function();
  else
    if (table_arg->default_field &&
        table_arg->update_default_fields(ignore_errors))
      goto err;

  if (table_arg->versioned() && !only_unvers_fields)
    table_arg->vers_update_fields();
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