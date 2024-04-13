void Item_func_in::mark_as_condition_AND_part(TABLE_LIST *embedding)
{
  THD *thd= current_thd;

  Query_arena *arena, backup;
  arena= thd->activate_stmt_arena_if_needed(&backup);

  if (!transform_into_subq_checked)
  {
    if ((transform_into_subq= to_be_transformed_into_in_subq(thd)))
      thd->lex->current_select->in_funcs.push_back(this, thd->mem_root);
    transform_into_subq_checked= true;
  }

  if (arena)
    thd->restore_active_arena(arena, &backup);

  emb_on_expr_nest= embedding;
}