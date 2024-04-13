void LEX::reset_arena_for_set_stmt(Query_arena *backup)
{
  DBUG_ENTER("LEX::reset_arena_for_set_stmt");
  DBUG_ASSERT(arena_for_set_stmt);
  thd->restore_active_arena(arena_for_set_stmt, backup);
  DBUG_PRINT("info", ("mem_root: %p  arena: %p",
                      arena_for_set_stmt->mem_root,
                      arena_for_set_stmt));
  DBUG_VOID_RETURN;
}