bool JOIN_TAB::preread_init()
{
  TABLE_LIST *derived= table->pos_in_table_list;
  DBUG_ENTER("JOIN_TAB::preread_init");

  if (!derived || !derived->is_materialized_derived())
  {
    preread_init_done= TRUE;
    DBUG_RETURN(FALSE);
  }

  /* Materialize derived table/view. */
  if ((!derived->get_unit()->executed  ||
       derived->is_recursive_with_table()) &&
      mysql_handle_single_derived(join->thd->lex,
                                    derived, DT_CREATE | DT_FILL))
      DBUG_RETURN(TRUE);

  preread_init_done= TRUE;
  if (select && select->quick)
    select->quick->replace_handler(table->file);

  DBUG_EXECUTE_IF("show_explain_probe_join_tab_preread", 
                  if (dbug_user_var_equals_int(join->thd, 
                                               "show_explain_probe_select_id", 
                                               join->select_lex->select_number))
                        dbug_serve_apcs(join->thd, 1);
                 );

  /* init ftfuns for just initialized derived table */
  if (table->fulltext_searched)
    init_ftfuncs(join->thd, join->select_lex, MY_TEST(join->order));

  DBUG_RETURN(FALSE);
}