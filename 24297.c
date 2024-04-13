static void update_field_dependencies(THD *thd, Field *field, TABLE *table)
{
  DBUG_ENTER("update_field_dependencies");
  if (thd->mark_used_columns != MARK_COLUMNS_NONE)
  {
    MY_BITMAP *bitmap;

    /*
      We always want to register the used keys, as the column bitmap may have
      been set for all fields (for example for view).
    */
      
    table->covering_keys.intersect(field->part_of_key);

    if (field->vcol_info)
      table->mark_virtual_col(field);

    if (thd->mark_used_columns == MARK_COLUMNS_READ)
      bitmap= table->read_set;
    else
      bitmap= table->write_set;

    /* 
       The test-and-set mechanism in the bitmap is not reliable during
       multi-UPDATE statements under MARK_COLUMNS_READ mode
       (thd->mark_used_columns == MARK_COLUMNS_READ), as this bitmap contains
       only those columns that are used in the SET clause. I.e they are being
       set here. See multi_update::prepare()
    */
    if (bitmap_fast_test_and_set(bitmap, field->field_index))
    {
      if (thd->mark_used_columns == MARK_COLUMNS_WRITE)
      {
        DBUG_PRINT("warning", ("Found duplicated field"));
        thd->dup_field= field;
      }
      else
      {
        DBUG_PRINT("note", ("Field found before"));
      }
      DBUG_VOID_RETURN;
    }
    if (table->get_fields_in_item_tree)
      field->flags|= GET_FIXED_FIELDS_FLAG;
    table->used_fields++;
  }
  else if (table->get_fields_in_item_tree)
    field->flags|= GET_FIXED_FIELDS_FLAG;
  DBUG_VOID_RETURN;
}