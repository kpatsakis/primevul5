static void prepare_record_for_error_message(int error, TABLE *table)
{
  Field **field_p;
  Field *field;
  uint keynr;
  MY_BITMAP unique_map; /* Fields in offended unique. */
  my_bitmap_map unique_map_buf[bitmap_buffer_size(MAX_FIELDS)];
  DBUG_ENTER("prepare_record_for_error_message");

  /*
    Only duplicate key errors print the key value.
    If storage engine does always read all columns, we have the value alraedy.
  */
  if ((error != HA_ERR_FOUND_DUPP_KEY) ||
      !(table->file->ha_table_flags() & HA_PARTIAL_COLUMN_READ))
    DBUG_VOID_RETURN;

  /*
    Get the number of the offended index.
    We will see MAX_KEY if the engine cannot determine the affected index.
  */
  if (unlikely((keynr= table->file->get_dup_key(error)) >= MAX_KEY))
    DBUG_VOID_RETURN;

  /* Create unique_map with all fields used by that index. */
  my_bitmap_init(&unique_map, unique_map_buf, table->s->fields, FALSE);
  table->mark_index_columns(keynr, &unique_map);

  /* Subtract read_set and write_set. */
  bitmap_subtract(&unique_map, table->read_set);
  bitmap_subtract(&unique_map, table->write_set);

  /*
    If the unique index uses columns that are neither in read_set
    nor in write_set, we must re-read the record.
    Otherwise no need to do anything.
  */
  if (bitmap_is_clear_all(&unique_map))
    DBUG_VOID_RETURN;

  /* Get identifier of last read record into table->file->ref. */
  table->file->position(table->record[0]);
  /* Add all fields used by unique index to read_set. */
  bitmap_union(table->read_set, &unique_map);
  /* Tell the engine about the new set. */
  table->file->column_bitmaps_signal();

  if ((error= table->file->ha_index_or_rnd_end()) ||
      (error= table->file->ha_rnd_init(0)))
  {
    table->file->print_error(error, MYF(0));
    DBUG_VOID_RETURN;
  }

  /* Read record that is identified by table->file->ref. */
  (void) table->file->ha_rnd_pos(table->record[1], table->file->ref);
  /* Copy the newly read columns into the new record. */
  for (field_p= table->field; (field= *field_p); field_p++)
    if (bitmap_is_set(&unique_map, field->field_index))
      field->copy_from_tmp(table->s->rec_buff_length);

  DBUG_VOID_RETURN;
}