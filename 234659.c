end_write_group(JOIN *join, JOIN_TAB *join_tab __attribute__((unused)),
		bool end_of_records)
{
  TABLE *table= join_tab->table;
  int	  idx= -1;
  DBUG_ENTER("end_write_group");

  if (!join->first_record || end_of_records ||
      (idx=test_if_group_changed(join->group_fields)) >= 0)
  {
    if (join->first_record || (end_of_records && !join->group))
    {
      if (join->procedure)
	join->procedure->end_group();
      int send_group_parts= join->send_group_parts;
      if (idx < send_group_parts)
      {
        if (!join->first_record)
        {
          /* No matching rows for group function */
          join->clear();
        }
        copy_sum_funcs(join->sum_funcs,
                       join->sum_funcs_end[send_group_parts]);
	if (!join_tab->having || join_tab->having->val_int())
	{
          int error= table->file->ha_write_tmp_row(table->record[0]);
          if (unlikely(error) &&
              create_internal_tmp_table_from_heap(join->thd, table,
                                          join_tab->tmp_table_param->start_recinfo,
                                          &join_tab->tmp_table_param->recinfo,
                                                   error, 0, NULL))
	    DBUG_RETURN(NESTED_LOOP_ERROR);
        }
        if (unlikely(join->rollup.state != ROLLUP::STATE_NONE))
	{
          if (unlikely(join->rollup_write_data((uint) (idx+1),
                                               join_tab->tmp_table_param,
                                               table)))
          {
	    DBUG_RETURN(NESTED_LOOP_ERROR);
          }
	}
	if (end_of_records)
	  goto end;
      }
    }
    else
    {
      if (end_of_records)
        goto end;
      join->first_record=1;
      (void) test_if_group_changed(join->group_fields);
    }
    if (idx < (int) join->send_group_parts)
    {
      copy_fields(join_tab->tmp_table_param);
      if (unlikely(copy_funcs(join_tab->tmp_table_param->items_to_copy,
                              join->thd)))
	DBUG_RETURN(NESTED_LOOP_ERROR);
      if (unlikely(init_sum_functions(join->sum_funcs,
                                      join->sum_funcs_end[idx+1])))
	DBUG_RETURN(NESTED_LOOP_ERROR);
      if (unlikely(join->procedure))
	join->procedure->add();
      goto end;
    }
  }
  if (unlikely(update_sum_func(join->sum_funcs)))
    DBUG_RETURN(NESTED_LOOP_ERROR);
  if (unlikely(join->procedure))
    join->procedure->add();
end:
  if (unlikely(join->thd->check_killed()))
  {
    DBUG_RETURN(NESTED_LOOP_KILLED);             /* purecov: inspected */
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}