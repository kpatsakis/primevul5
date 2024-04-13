my_bool ha_maria::register_query_cache_table(THD *thd, const char *table_name,
					     uint table_name_len,
					     qc_engine_callback
					     *engine_callback,
					     ulonglong *engine_data)
{
  ulonglong actual_data_file_length;
  ulonglong current_data_file_length;
  DBUG_ENTER("ha_maria::register_query_cache_table");

  /*
    No call back function is needed to determine if a cached statement
    is valid or not.
  */
  *engine_callback= 0;

  /*
    No engine data is needed.
  */
  *engine_data= 0;

  if (file->s->now_transactional && file->s->have_versioning)
    DBUG_RETURN(file->trn->trid >= file->s->state.last_change_trn);

  /*
    If a concurrent INSERT has happened just before the currently processed
    SELECT statement, the total size of the table is unknown.

    To determine if the table size is known, the current thread's snap shot of
    the table size with the actual table size are compared.

    If the table size is unknown the SELECT statement can't be cached.
  */

  /*
    POSIX visibility rules specify that "2. Whatever memory values a
    thread can see when it unlocks a mutex <...> can also be seen by any
    thread that later locks the same mutex". In this particular case,
    concurrent insert thread had modified the data_file_length in
    MYISAM_SHARE before it has unlocked (or even locked)
    structure_guard_mutex. So, here we're guaranteed to see at least that
    value after we've locked the same mutex. We can see a later value
    (modified by some other thread) though, but it's ok, as we only want
    to know if the variable was changed, the actual new value doesn't matter
  */
  actual_data_file_length= file->s->state.state.data_file_length;
  current_data_file_length= file->state->data_file_length;

  /* Return whether is ok to try to cache current statement. */
  DBUG_RETURN(!(file->s->non_transactional_concurrent_insert &&
                current_data_file_length != actual_data_file_length));
}