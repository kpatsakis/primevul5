fill_record_n_invoke_before_triggers(THD *thd, TABLE *table,
                                     List<Item> &fields,
                                     List<Item> &values, bool ignore_errors,
                                     enum trg_event_type event)
{
  bool result;
  Table_triggers_list *triggers= table->triggers;

  result= fill_record(thd, table, fields, values, ignore_errors,
                      event == TRG_EVENT_UPDATE);

  if (!result && triggers)
  {
    if (triggers->process_triggers(thd, event, TRG_ACTION_BEFORE,
                                    TRUE) ||
        not_null_fields_have_null_values(table))
      return TRUE;

    /*
      Re-calculate virtual fields to cater for cases when base columns are
      updated by the triggers.
    */
    if (table->vfield && fields.elements)
    {
      Item *fld= (Item_field*) fields.head();
      Item_field *item_field= fld->field_for_view_update();
      if (item_field)
      {
        DBUG_ASSERT(table == item_field->field->table);
        result|= table->update_virtual_fields(table->file,
                                              VCOL_UPDATE_FOR_WRITE);
      }
    }
  }
  return result;
}