static void update_field_dependencies(THD *thd, Field *field, TABLE *table)
{
  DBUG_ENTER("update_field_dependencies");
  if (should_mark_column(thd->column_usage))
  {
    /*
      We always want to register the used keys, as the column bitmap may have
      been set for all fields (for example for view).
    */
    table->covering_keys.intersect(field->part_of_key);

    if (thd->column_usage == MARK_COLUMNS_READ)
    {
      if (table->mark_column_with_deps(field))
        DBUG_VOID_RETURN; // Field was already marked
    }
    else
    {
      if (bitmap_fast_test_and_set(table->write_set, field->field_index))
      {
        DBUG_PRINT("warning", ("Found duplicated field"));
        thd->dup_field= field;
        DBUG_VOID_RETURN;
      }
    }

    table->used_fields++;
  }
  if (table->get_fields_in_item_tree)
    field->flags|= GET_FIXED_FIELDS_FLAG;
  DBUG_VOID_RETURN;
}