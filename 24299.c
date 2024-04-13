prepare_fk_prelocking_list(THD *thd, Query_tables_list *prelocking_ctx,
                           TABLE_LIST *table_list, bool *need_prelocking,
                           uint8 op)
{
  List <FOREIGN_KEY_INFO> fk_list;
  List_iterator<FOREIGN_KEY_INFO> fk_list_it(fk_list);
  FOREIGN_KEY_INFO *fk;
  Query_arena *arena, backup;

  arena= thd->activate_stmt_arena_if_needed(&backup);

  table_list->table->file->get_parent_foreign_key_list(thd, &fk_list);
  if (thd->is_error())
  {
    if (arena)
      thd->restore_active_arena(arena, &backup);
    return TRUE;
  }

  *need_prelocking= TRUE;

  while ((fk= fk_list_it++))
  {
    // FK_OPTION_RESTRICT and FK_OPTION_NO_ACTION only need read access
    static bool can_write[]= { true, false, true, true, false, true };
    thr_lock_type lock_type;

    if ((op & (1 << TRG_EVENT_DELETE) && can_write[fk->delete_method])
        || (op & (1 << TRG_EVENT_UPDATE) && can_write[fk->update_method]))
      lock_type= TL_WRITE_ALLOW_WRITE;
    else
      lock_type= TL_READ;

    if (table_already_fk_prelocked(prelocking_ctx->query_tables,
          fk->foreign_db, fk->foreign_table,
          lock_type))
      continue;

    TABLE_LIST *tl= (TABLE_LIST *) thd->alloc(sizeof(TABLE_LIST));
    tl->init_one_table_for_prelocking(fk->foreign_db->str, fk->foreign_db->length,
        fk->foreign_table->str, fk->foreign_table->length,
        NULL, lock_type, false, table_list->belong_to_view,
        op, &prelocking_ctx->query_tables_last);
  }
  if (arena)
    thd->restore_active_arena(arena, &backup);

  return FALSE;
}