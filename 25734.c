bool mysql_prepare_update(THD *thd, TABLE_LIST *table_list,
			 Item **conds, uint order_num, ORDER *order)
{
  Item *fake_conds= 0;
#ifndef NO_EMBEDDED_ACCESS_CHECKS
  TABLE *table= table_list->table;
#endif
  List<Item> all_fields;
  SELECT_LEX *select_lex= &thd->lex->select_lex;
  DBUG_ENTER("mysql_prepare_update");

#ifndef NO_EMBEDDED_ACCESS_CHECKS
  table_list->grant.want_privilege= table->grant.want_privilege= 
    (SELECT_ACL & ~table->grant.privilege);
  table_list->register_want_access(SELECT_ACL);
#endif

  thd->lex->allow_sum_func.clear_all();

  DBUG_ASSERT(table_list->table);
  // conds could be cached from previous SP call
  DBUG_ASSERT(!table_list->vers_conditions.need_setup() ||
              !*conds || thd->stmt_arena->is_stmt_execute());
  if (select_lex->vers_setup_conds(thd, table_list))
    DBUG_RETURN(TRUE);

  *conds= select_lex->where;

  /*
    We do not call DT_MERGE_FOR_INSERT because it has no sense for simple
    (not multi-) update
  */
  if (mysql_handle_derived(thd->lex, DT_PREPARE))
    DBUG_RETURN(TRUE);

  if (setup_tables_and_check_access(thd, &select_lex->context, 
                                    &select_lex->top_join_list, table_list,
                                    select_lex->leaf_tables,
                                    FALSE, UPDATE_ACL, SELECT_ACL, TRUE) ||
      setup_conds(thd, table_list, select_lex->leaf_tables, conds) ||
      select_lex->setup_ref_array(thd, order_num) ||
      setup_order(thd, select_lex->ref_pointer_array,
		  table_list, all_fields, all_fields, order) ||
      setup_ftfuncs(select_lex))
    DBUG_RETURN(TRUE);

  select_lex->fix_prepare_information(thd, conds, &fake_conds);
  DBUG_RETURN(FALSE);
}