bool LEX::save_prep_leaf_tables()
{
  if (!thd->save_prep_leaf_list)
    return FALSE;

  Query_arena *arena= thd->stmt_arena, backup;
  arena= thd->activate_stmt_arena_if_needed(&backup);
  //It is used for DETETE/UPDATE so top level has only one SELECT
  DBUG_ASSERT(select_lex.next_select() == NULL);
  bool res= select_lex.save_prep_leaf_tables(thd);

  if (arena)
    thd->restore_active_arena(arena, &backup);

  if (res)
    return TRUE;

  thd->save_prep_leaf_list= FALSE;
  return FALSE;
}