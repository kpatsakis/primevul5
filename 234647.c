end_unique_update(JOIN *join, JOIN_TAB *join_tab __attribute__((unused)),
		  bool end_of_records)
{
  TABLE *table= join_tab->table;
  int	  error;
  DBUG_ENTER("end_unique_update");

  if (end_of_records)
    DBUG_RETURN(NESTED_LOOP_OK);

  init_tmptable_sum_functions(join->sum_funcs);
  copy_fields(join_tab->tmp_table_param);		// Groups are copied twice.
  if (copy_funcs(join_tab->tmp_table_param->items_to_copy, join->thd))
    DBUG_RETURN(NESTED_LOOP_ERROR);           /* purecov: inspected */

  if (likely(!(error= table->file->ha_write_tmp_row(table->record[0]))))
    join_tab->send_records++;			// New group
  else
  {
    if (unlikely((int) table->file->get_dup_key(error) < 0))
    {
      table->file->print_error(error,MYF(0));	/* purecov: inspected */
      DBUG_RETURN(NESTED_LOOP_ERROR);            /* purecov: inspected */
    }
    /* Prepare table for random positioning */
    bool rnd_inited= (table->file->inited == handler::RND);
    if (!rnd_inited &&
        ((error= table->file->ha_index_end()) ||
         (error= table->file->ha_rnd_init(0))))
    {
      table->file->print_error(error, MYF(0));
      DBUG_RETURN(NESTED_LOOP_ERROR);
    }
    if (unlikely(table->file->ha_rnd_pos(table->record[1],table->file->dup_ref)))
    {
      table->file->print_error(error,MYF(0));	/* purecov: inspected */
      DBUG_RETURN(NESTED_LOOP_ERROR);            /* purecov: inspected */
    }
    restore_record(table,record[1]);
    update_tmptable_sum_func(join->sum_funcs,table);
    if (unlikely((error= table->file->ha_update_tmp_row(table->record[1],
                                                        table->record[0]))))
    {
      table->file->print_error(error,MYF(0));	/* purecov: inspected */
      DBUG_RETURN(NESTED_LOOP_ERROR);            /* purecov: inspected */
    }
    if (!rnd_inited &&
        ((error= table->file->ha_rnd_end()) ||
         (error= table->file->ha_index_init(0, 0))))
    {
      table->file->print_error(error, MYF(0));
      DBUG_RETURN(NESTED_LOOP_ERROR);
    }
  }
  if (unlikely(join->thd->check_killed()))
  {
    DBUG_RETURN(NESTED_LOOP_KILLED);             /* purecov: inspected */
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}