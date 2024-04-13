end_update(JOIN *join, JOIN_TAB *join_tab __attribute__((unused)),
	   bool end_of_records)
{
  TABLE *const table= join_tab->table;
  ORDER   *group;
  int	  error;
  DBUG_ENTER("end_update");

  if (end_of_records)
    DBUG_RETURN(NESTED_LOOP_OK);

  join->found_records++;
  copy_fields(join_tab->tmp_table_param);	// Groups are copied twice.
  /* Make a key of group index */
  for (group=table->group ; group ; group=group->next)
  {
    Item *item= *group->item;
    if (group->fast_field_copier_setup != group->field)
    {
      DBUG_PRINT("info", ("new setup %p -> %p",
                          group->fast_field_copier_setup,
                          group->field));
      group->fast_field_copier_setup= group->field;
      group->fast_field_copier_func=
        item->setup_fast_field_copier(group->field);
    }
    item->save_org_in_field(group->field, group->fast_field_copier_func);
    /* Store in the used key if the field was 0 */
    if (item->maybe_null)
      group->buff[-1]= (char) group->field->is_null();
  }
  if (!table->file->ha_index_read_map(table->record[1],
                                      join_tab->tmp_table_param->group_buff,
                                      HA_WHOLE_KEY,
                                      HA_READ_KEY_EXACT))
  {						/* Update old record */
    restore_record(table,record[1]);
    update_tmptable_sum_func(join->sum_funcs,table);
    if (unlikely((error= table->file->ha_update_tmp_row(table->record[1],
                                                        table->record[0]))))
    {
      table->file->print_error(error,MYF(0));	/* purecov: inspected */
      DBUG_RETURN(NESTED_LOOP_ERROR);            /* purecov: inspected */
    }
    goto end;
  }

  init_tmptable_sum_functions(join->sum_funcs);
  if (unlikely(copy_funcs(join_tab->tmp_table_param->items_to_copy,
                          join->thd)))
    DBUG_RETURN(NESTED_LOOP_ERROR);           /* purecov: inspected */
  if (unlikely((error= table->file->ha_write_tmp_row(table->record[0]))))
  {
    if (create_internal_tmp_table_from_heap(join->thd, table,
                                       join_tab->tmp_table_param->start_recinfo,
                                            &join_tab->tmp_table_param->recinfo,
                                            error, 0, NULL))
      DBUG_RETURN(NESTED_LOOP_ERROR);            // Not a table_is_full error
    /* Change method to update rows */
    if (unlikely((error= table->file->ha_index_init(0, 0))))
    {
      table->file->print_error(error, MYF(0));
      DBUG_RETURN(NESTED_LOOP_ERROR);
    }

    join_tab->aggr->set_write_func(end_unique_update);
  }
  join_tab->send_records++;
end:
  if (unlikely(join->thd->check_killed()))
  {
    DBUG_RETURN(NESTED_LOOP_KILLED);             /* purecov: inspected */
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}