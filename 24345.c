bool st_select_lex::save_leaf_tables(THD *thd)
{
  Query_arena *arena, backup;
  arena= thd->activate_stmt_arena_if_needed(&backup);

  List_iterator_fast<TABLE_LIST> li(leaf_tables);
  TABLE_LIST *table;
  while ((table= li++))
  {
    if (leaf_tables_exec.push_back(table, thd->mem_root))
      return 1;
    table->tablenr_exec= table->get_tablenr();
    table->map_exec= table->get_map();
    if (join && (join->select_options & SELECT_DESCRIBE))
      table->maybe_null_exec= 0;
    else
      table->maybe_null_exec= table->table?  table->table->maybe_null: 0;
  }
  if (arena)
    thd->restore_active_arena(arena, &backup);

  return 0;
}