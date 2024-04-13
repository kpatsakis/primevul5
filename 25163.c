static bool convert_const_to_int(THD *thd, Item_field *field_item,
                                  Item **item)
{
  Field *field= field_item->field;
  int result= 0;

  /*
    We don't need to convert an integer to an integer,
    pretend it's already converted.

    But we still convert it if it is compared with a Field_year,
    as YEAR(2) may change the value of an integer when converting it
    to an integer (say, 0 to 70).
  */
  if ((*item)->cmp_type() == INT_RESULT &&
      field_item->field_type() != MYSQL_TYPE_YEAR)
    return 1;

  if ((*item)->const_item() && !(*item)->is_expensive())
  {
    TABLE *table= field->table;
    Sql_mode_save sql_mode(thd);
    Check_level_instant_set check_level_save(thd, CHECK_FIELD_IGNORE);
    MY_BITMAP *old_maps[2] = { NULL, NULL };
    ulonglong UNINIT_VAR(orig_field_val); /* original field value if valid */

    /* table->read_set may not be set if we come here from a CREATE TABLE */
    if (table && table->read_set)
      dbug_tmp_use_all_columns(table, old_maps,
                               &table->read_set, &table->write_set);
    /* For comparison purposes allow invalid dates like 2000-01-32 */
    thd->variables.sql_mode= (thd->variables.sql_mode & ~MODE_NO_ZERO_DATE) |
                             MODE_INVALID_DATES;

    /*
      Store the value of the field/constant because the call to save_in_field
      below overrides that value. Don't save field value if no data has been
      read yet.
    */
    bool save_field_value= (field_item->const_item() ||
                            !(field->table->status & STATUS_NO_RECORD));
    if (save_field_value)
      orig_field_val= field->val_int();
    if (!(*item)->save_in_field(field, 1) && !field->is_null())
    {
      int field_cmp= 0;
      // If item is a decimal value, we must reject it if it was truncated.
      if (field->type() == MYSQL_TYPE_LONGLONG)
      {
        field_cmp= stored_field_cmp_to_item(thd, field, *item);
        DBUG_PRINT("info", ("convert_const_to_int %d", field_cmp));
      }

      if (0 == field_cmp)
      {
        Item *tmp= new (thd->mem_root) Item_int_with_ref(thd, field->val_int(), *item,
                                         MY_TEST(field->flags & UNSIGNED_FLAG));
        if (tmp)
          thd->change_item_tree(item, tmp);
        result= 1;					// Item was replaced
      }
    }
    /* Restore the original field value. */
    if (save_field_value)
    {
      result= field->store(orig_field_val, TRUE);
      /* orig_field_val must be a valid value that can be restored back. */
      DBUG_ASSERT(!result);
    }
    if (table && table->read_set)
      dbug_tmp_restore_column_maps(&table->read_set, &table->write_set, old_maps);
  }
  return result;
}