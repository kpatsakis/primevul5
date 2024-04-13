bool flush_tables(THD *thd, flush_tables_type flag)
{
  bool result= TRUE;
  tc_collect_arg collect_arg;
  TABLE *tmp_table;
  flush_tables_error_handler error_handler;
  DBUG_ENTER("flush_tables");

  purge_tables(false);  /* Flush unused tables and shares */

  /*
    Loop over all shares and collect shares that have open tables
    TODO:
    Optimize this to only collect shares that have been used for
    write after last time all tables was closed.
  */

  if (!(tmp_table= (TABLE*) my_malloc(sizeof(*tmp_table),
                                      MYF(MY_WME | MY_THREAD_SPECIFIC))))
    DBUG_RETURN(1);

  my_init_dynamic_array(&collect_arg.shares, sizeof(TABLE_SHARE*), 100, 100,
                        MYF(0));
  collect_arg.flush_type= flag;
  if (tdc_iterate(thd, (my_hash_walk_action) tc_collect_used_shares,
                  &collect_arg, true))
  {
    /* Release already collected shares */
    for (uint i= 0 ; i < collect_arg.shares.elements ; i++)
    {
      TABLE_SHARE *share= *dynamic_element(&collect_arg.shares, i,
                                           TABLE_SHARE**);
      tdc_release_share(share);
    }
    goto err;
  }

  /* Call HA_EXTRA_FLUSH on all found shares */

  thd->push_internal_handler(&error_handler);
  for (uint i= 0 ; i < collect_arg.shares.elements ; i++)
  {
    TABLE_SHARE *share= *dynamic_element(&collect_arg.shares, i,
                                         TABLE_SHARE**);
    TABLE *table= tc_acquire_table(thd, share->tdc);
    if (table)
    {
      (void) table->file->extra(HA_EXTRA_FLUSH);
      tc_release_table(table);
    }
    else
    {
      /*
        HA_OPEN_FOR_ALTER is used to allow us to open the table even if
        TABLE_SHARE::incompatible_version is set.
      */
      if (!open_table_from_share(thd, share, &empty_clex_str,
                                 HA_OPEN_KEYFILE, 0,
                                 HA_OPEN_FOR_ALTER,
                                 tmp_table, FALSE,
                                 NULL))
      {
        (void) tmp_table->file->extra(HA_EXTRA_FLUSH);
        /*
          We don't put the table into the TDC as the table was not fully
          opened (we didn't open triggers)
        */
        closefrm(tmp_table);
      }
    }
    tdc_release_share(share);
  }
  thd->pop_internal_handler();
  result= error_handler.got_fatal_error();
  DBUG_PRINT("note", ("open_errors: %u %u",
                      error_handler.handled_errors,
                      error_handler.unhandled_errors));
err:
  my_free(tmp_table);
  delete_dynamic(&collect_arg.shares);
  DBUG_RETURN(result);
}