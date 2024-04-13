int ha_maria::end_bulk_insert()
{
  int first_error, error;
  my_bool abort= file->s->deleting;
  DBUG_ENTER("ha_maria::end_bulk_insert");

  if ((first_error= maria_end_bulk_insert(file, abort)))
    abort= 1;

  if ((error= maria_extra(file, HA_EXTRA_NO_CACHE, 0)))
  {
    first_error= first_error ? first_error : error;
    abort= 1;
  }

  if (!abort && can_enable_indexes)
    if ((error= enable_indexes(HA_KEY_SWITCH_NONUNIQ_SAVE)))
      first_error= first_error ? first_error : error;

  if (bulk_insert_single_undo != BULK_INSERT_NONE)
  {
    /*
      Table was transactional just before start_bulk_insert().
      No need to flush pages if we did a repair (which already flushed).
    */
    if ((error= _ma_reenable_logging_for_table(file,
                                               bulk_insert_single_undo ==
                                               BULK_INSERT_SINGLE_UNDO_AND_NO_REPAIR)))
      first_error= first_error ? first_error : error;
    bulk_insert_single_undo= BULK_INSERT_NONE;  // Safety
  }
  can_enable_indexes= 0;
  DBUG_RETURN(first_error);
}