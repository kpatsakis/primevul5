bool Item_func_not::fix_fields(THD *thd, Item **ref)
{
  args[0]->under_not(this);
  if (args[0]->type() == FIELD_ITEM)
  {
    /* replace  "NOT <field>" with "<field> == 0" */
    Query_arena backup, *arena;
    Item *new_item;
    bool rc= TRUE;
    arena= thd->activate_stmt_arena_if_needed(&backup);
    if ((new_item= new (thd->mem_root) Item_func_eq(thd, args[0], new (thd->mem_root) Item_int(thd, 0, 1))))
    {
      new_item->name= name;
      rc= (*ref= new_item)->fix_fields(thd, ref);
    }
    if (arena)
      thd->restore_active_arena(arena, &backup);
    return rc;
  }
  return Item_func::fix_fields(thd, ref);
}