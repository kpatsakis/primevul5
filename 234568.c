end_sj_materialize(JOIN *join, JOIN_TAB *join_tab, bool end_of_records)
{
  int error;
  THD *thd= join->thd;
  SJ_MATERIALIZATION_INFO *sjm= join_tab[-1].emb_sj_nest->sj_mat_info;
  DBUG_ENTER("end_sj_materialize");
  if (!end_of_records)
  {
    TABLE *table= sjm->table;

    List_iterator<Item> it(sjm->sjm_table_cols);
    Item *item;
    while ((item= it++))
    {
      if (item->is_null())
        DBUG_RETURN(NESTED_LOOP_OK);
    }
    fill_record(thd, table, table->field, sjm->sjm_table_cols, TRUE, FALSE);
    if (unlikely(thd->is_error()))
      DBUG_RETURN(NESTED_LOOP_ERROR); /* purecov: inspected */
    if (unlikely((error= table->file->ha_write_tmp_row(table->record[0]))))
    {
      /* create_myisam_from_heap will generate error if needed */
      if (table->file->is_fatal_error(error, HA_CHECK_DUP) &&
          create_internal_tmp_table_from_heap(thd, table,
                                              sjm->sjm_table_param.start_recinfo, 
                                              &sjm->sjm_table_param.recinfo, error, 1, NULL))
        DBUG_RETURN(NESTED_LOOP_ERROR); /* purecov: inspected */
    }
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}