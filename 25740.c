int mysql_multi_update_prepare(THD *thd)
{
  LEX *lex= thd->lex;
  TABLE_LIST *table_list= lex->query_tables;
  TABLE_LIST *tl;
  Multiupdate_prelocking_strategy prelocking_strategy;
  uint table_count= lex->table_count;
  DBUG_ENTER("mysql_multi_update_prepare");

  /*
    Open tables and create derived ones, but do not lock and fill them yet.

    During prepare phase acquire only S metadata locks instead of SW locks to
    keep prepare of multi-UPDATE compatible with concurrent LOCK TABLES WRITE
    and global read lock.

    Don't evaluate any subqueries even if constant, because
    tables aren't locked yet.
  */
  lex->context_analysis_only|= CONTEXT_ANALYSIS_ONLY_DERIVED;
  if (thd->lex->sql_command == SQLCOM_UPDATE_MULTI)
  {
    if (open_tables(thd, &table_list, &table_count,
        thd->stmt_arena->is_stmt_prepare() ? MYSQL_OPEN_FORCE_SHARED_MDL : 0,
        &prelocking_strategy))
      DBUG_RETURN(TRUE);
  }
  else
  {
    /* following need for prepared statements, to run next time multi-update */
    thd->lex->sql_command= SQLCOM_UPDATE_MULTI;
    prelocking_strategy.reset(thd);
    if (prelocking_strategy.handle_end(thd))
      DBUG_RETURN(TRUE);
  }

  /* now lock and fill tables */
  if (!thd->stmt_arena->is_stmt_prepare() &&
      lock_tables(thd, table_list, table_count, 0))
  {
    DBUG_RETURN(TRUE);
  }

  lex->context_analysis_only&= ~CONTEXT_ANALYSIS_ONLY_DERIVED;

  (void) read_statistics_for_tables_if_needed(thd, table_list);
  /* @todo: downgrade the metadata locks here. */

  /*
    Check that we are not using table that we are updating, but we should
    skip all tables of UPDATE SELECT itself
  */
  lex->select_lex.exclude_from_table_unique_test= TRUE;

  /* We only need SELECT privilege for columns in the values list */
  List_iterator<TABLE_LIST> ti(lex->select_lex.leaf_tables);
  while ((tl= ti++))
  {
    if (tl->is_jtbm())
      continue;
    TABLE *table= tl->table;
    TABLE_LIST *tlist;
    if (!(tlist= tl->top_table())->derived)
    {
      tlist->grant.want_privilege=
        (SELECT_ACL & ~tlist->grant.privilege);
      table->grant.want_privilege= (SELECT_ACL & ~table->grant.privilege);
    }
    DBUG_PRINT("info", ("table: %s  want_privilege: %u", tl->alias.str,
                        (uint) table->grant.want_privilege));
  }
  /*
    Set exclude_from_table_unique_test value back to FALSE. It is needed for
    further check in multi_update::prepare whether to use record cache.
  */
  lex->select_lex.exclude_from_table_unique_test= FALSE;

  if (lex->save_prep_leaf_tables())
    DBUG_RETURN(TRUE);
 
  DBUG_RETURN (FALSE);
}