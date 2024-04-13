void wrap_ident(THD *thd, Item **conds)
{
  Item_direct_ref_to_ident *wrapper;
  DBUG_ASSERT((*conds)->type() == Item::FIELD_ITEM || (*conds)->type() == Item::REF_ITEM);
  Query_arena *arena, backup;
  arena= thd->activate_stmt_arena_if_needed(&backup);
  if ((wrapper= new (thd->mem_root) Item_direct_ref_to_ident(thd, (Item_ident *) (*conds))))
    (*conds)= (Item*) wrapper;
  if (arena)
    thd->restore_active_arena(arena, &backup);
}