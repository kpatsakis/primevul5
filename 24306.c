TABLE_LIST* find_dup_table(THD *thd, TABLE_LIST *table, TABLE_LIST *table_list,
                           uint check_flag)
{
  TABLE_LIST *res= 0;
  const char *d_name, *t_name, *t_alias;
  DBUG_ENTER("find_dup_table");
  DBUG_PRINT("enter", ("table alias: %s", table->alias));

  /*
    If this function called for query which update table (INSERT/UPDATE/...)
    then we have in table->table pointer to TABLE object which we are
    updating even if it is VIEW so we need TABLE_LIST of this TABLE object
    to get right names (even if lower_case_table_names used).

    If this function called for CREATE command that we have not opened table
    (table->table equal to 0) and right names is in current TABLE_LIST
    object.
  */
  if (table->table)
  {
    /* All MyISAMMRG children are plain MyISAM tables. */
    DBUG_ASSERT(table->table->file->ht->db_type != DB_TYPE_MRG_MYISAM);

    table= table->find_underlying_table(table->table);
    /*
      as far as we have table->table we have to find real TABLE_LIST of
      it in underlying tables
    */
    DBUG_ASSERT(table);
  }
  d_name= table->db;
  t_name= table->table_name;
  t_alias= table->alias;

retry:
  DBUG_PRINT("info", ("real table: %s.%s", d_name, t_name));
  for (TABLE_LIST *tl= table_list; tl ; tl= tl->next_global, res= 0)
  {
    if (tl->select_lex && tl->select_lex->master_unit() &&
        tl->select_lex->master_unit()->executed)
    {
      /*
        There is no sense to check tables of already executed parts
        of the query
      */
      continue;
    }
    /*
      Table is unique if it is present only once in the global list
      of tables and once in the list of table locks.
    */
    if (! (res= find_table_in_global_list(tl, d_name, t_name)))
      break;
    tl= res;                       // We can continue search after this table

    /* Skip if same underlying table. */
    if (res->table && (res->table == table->table))
      continue;

    /* Skip if table is tmp table */
    if (check_flag & CHECK_DUP_SKIP_TEMP_TABLE &&
        res->table && res->table->s->tmp_table != NO_TMP_TABLE)
    {
      continue;
    }
    if (check_flag & CHECK_DUP_FOR_CREATE)
      DBUG_RETURN(res);

    /* Skip if table alias does not match. */
    if (check_flag & CHECK_DUP_ALLOW_DIFFERENT_ALIAS)
    {
      if (my_strcasecmp(table_alias_charset, t_alias, res->alias))
        continue;
    }

    /*
      If table is not excluded (could be a derived table) and table is not
      a prelocking placeholder then we found either a duplicate entry
      or a table that is part of a derived table (handled below).
      Examples are:
      INSERT INTO t1 SELECT * FROM t1;
      INSERT INTO t1 SELECT * FROM view_containing_t1;
    */
    if (res->select_lex &&
        !res->select_lex->exclude_from_table_unique_test &&
        !res->prelocking_placeholder)
      break;

    /*
      If we found entry of this table or table of SELECT which already
      processed in derived table or top select of multi-update/multi-delete
      (exclude_from_table_unique_test) or prelocking placeholder.
    */
    DBUG_PRINT("info",
               ("found same copy of table or table which we should skip"));
  }
  if (res && res->belong_to_derived)
  {
    /*
      We come here for queries of type:
      INSERT INTO t1 (SELECT tmp.a FROM (select * FROM t1) as tmp);

      Try to fix by materializing the derived table
    */
    TABLE_LIST *derived=  res->belong_to_derived;
    if (derived->is_merged_derived() && !derived->derived->is_excluded())
    {
      DBUG_PRINT("info",
                 ("convert merged to materialization to resolve the conflict"));
      derived->change_refs_to_fields();
      derived->set_materialized_derived();
      goto retry;
    }
  }
  DBUG_RETURN(res);
}