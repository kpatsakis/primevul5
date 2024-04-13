bool Multiupdate_prelocking_strategy::handle_end(THD *thd)
{
  DBUG_ENTER("Multiupdate_prelocking_strategy::handle_end");
  if (done)
    DBUG_RETURN(0);

  LEX *lex= thd->lex;
  SELECT_LEX *select_lex= &lex->select_lex;
  TABLE_LIST *table_list= lex->query_tables, *tl;

  done= true;

  if (mysql_handle_derived(lex, DT_INIT) ||
      mysql_handle_derived(lex, DT_MERGE_FOR_INSERT) ||
      mysql_handle_derived(lex, DT_PREPARE))
    DBUG_RETURN(1);

  /*
    setup_tables() need for VIEWs. JOIN::prepare() will call setup_tables()
    second time, but this call will do nothing (there are check for second
    call in setup_tables()).
  */

  if (setup_tables_and_check_access(thd, &select_lex->context,
      &select_lex->top_join_list, table_list, select_lex->leaf_tables,
      FALSE, UPDATE_ACL, SELECT_ACL, TRUE))
    DBUG_RETURN(1);

  List<Item> *fields= &lex->select_lex.item_list;
  if (setup_fields_with_no_wrap(thd, Ref_ptr_array(),
                                *fields, MARK_COLUMNS_WRITE, 0, 0))
    DBUG_RETURN(1);

  // Check if we have a view in the list ...
  for (tl= table_list; tl ; tl= tl->next_local)
    if (tl->view)
      break;
  // ... and pass this knowlage in check_fields call
  if (check_fields(thd, *fields, tl != NULL ))
    DBUG_RETURN(1);

  table_map tables_for_update= thd->table_map_for_update= get_table_map(fields);

  if (unsafe_key_update(select_lex->leaf_tables, tables_for_update))
    DBUG_RETURN(1);

  /*
    Setup timestamp handling and locking mode
  */
  List_iterator<TABLE_LIST> ti(select_lex->leaf_tables);
  const bool using_lock_tables= thd->locked_tables_mode != LTM_NONE;
  while ((tl= ti++))
  {
    TABLE *table= tl->table;

    if (tl->is_jtbm())
      continue;

    /* if table will be updated then check that it is unique */
    if (table->map & tables_for_update)
    {
      if (!tl->single_table_updatable() || check_key_in_view(thd, tl))
      {
        my_error(ER_NON_UPDATABLE_TABLE, MYF(0),
                 tl->top_table()->alias.str, "UPDATE");
        DBUG_RETURN(1);
      }

      DBUG_PRINT("info",("setting table `%s` for update",
                         tl->top_table()->alias.str));
      /*
        If table will be updated we should not downgrade lock for it and
        leave it as is.
      */
      tl->updating= 1;
      if (tl->belong_to_view)
        tl->belong_to_view->updating= 1;
      if (extend_table_list(thd, tl, this, has_prelocking_list))
        DBUG_RETURN(1);
    }
    else
    {
      DBUG_PRINT("info",("setting table `%s` for read-only", tl->alias.str));
      /*
        If we are using the binary log, we need TL_READ_NO_INSERT to get
        correct order of statements. Otherwise, we use a TL_READ lock to
        improve performance.
        We don't downgrade metadata lock from SW to SR in this case as
        there is no guarantee that the same ticket is not used by
        another table instance used by this statement which is going to
        be write-locked (for example, trigger to be invoked might try
        to update this table).
        Last argument routine_modifies_data for read_lock_type_for_table()
        is ignored, as prelocking placeholder will never be set here.
      */
      DBUG_ASSERT(tl->prelocking_placeholder == false);
      thr_lock_type lock_type= read_lock_type_for_table(thd, lex, tl, true);
      if (using_lock_tables)
        tl->lock_type= lock_type;
      else
        tl->set_lock_type(thd, lock_type);
    }
  }

  /*
    Check access privileges for tables being updated or read.
    Note that unlike in the above loop we need to iterate here not only
    through all leaf tables but also through all view hierarchy.
  */

  for (tl= table_list; tl; tl= tl->next_local)
  {
    bool not_used= false;
    if (tl->is_jtbm())
      continue;
    if (multi_update_check_table_access(thd, tl, tables_for_update, &not_used))
      DBUG_RETURN(TRUE);
  }

  /* check single table update for view compound from several tables */
  for (tl= table_list; tl; tl= tl->next_local)
  {
    TABLE_LIST *for_update= 0;
    if (tl->is_jtbm())
      continue;
    if (tl->is_merged_derived() &&
        tl->check_single_table(&for_update, tables_for_update, tl))
    {
      my_error(ER_VIEW_MULTIUPDATE, MYF(0), tl->view_db.str, tl->view_name.str);
      DBUG_RETURN(1);
    }
  }

  DBUG_RETURN(0);
}