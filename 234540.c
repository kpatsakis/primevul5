test_if_quick_select(JOIN_TAB *tab)
{
  DBUG_EXECUTE_IF("show_explain_probe_test_if_quick_select", 
                  if (dbug_user_var_equals_int(tab->join->thd, 
                                               "show_explain_probe_select_id", 
                                               tab->join->select_lex->select_number))
                        dbug_serve_apcs(tab->join->thd, 1);
                 );


  delete tab->select->quick;
  tab->select->quick=0;

  if (tab->table->file->inited != handler::NONE)
    tab->table->file->ha_index_or_rnd_end();

  int res= tab->select->test_quick_select(tab->join->thd, tab->keys,
                                          (table_map) 0, HA_POS_ERROR, 0,
                                          FALSE, /*remove where parts*/FALSE);
  if (tab->explain_plan && tab->explain_plan->range_checked_fer)
    tab->explain_plan->range_checked_fer->collect_data(tab->select->quick);

  return res;
}