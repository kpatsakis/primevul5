fill_record_n_invoke_before_triggers(THD *thd, TABLE *table, Field **ptr,
                                     List<Item> &values, bool ignore_errors,
                                     enum trg_event_type event)
{
  bool result;
  Table_triggers_list *triggers= table->triggers;

  result= fill_record(thd, table, ptr, values, ignore_errors, FALSE);

  if (!result && triggers && *ptr)
    result= triggers->process_triggers(thd, event, TRG_ACTION_BEFORE, TRUE) ||
            not_null_fields_have_null_values(table);
  /*
    Re-calculate virtual fields to cater for cases when base columns are
    updated by the triggers.
  */
  if (!result && triggers && *ptr)
  {
    DBUG_ASSERT(table == (*ptr)->table);
    if (table->vfield)
      result= table->update_virtual_fields(table->file, VCOL_UPDATE_FOR_WRITE);
  }
  return result;

}