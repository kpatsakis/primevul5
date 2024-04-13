void LEX::free_arena_for_set_stmt()
{
  DBUG_ENTER("LEX::free_arena_for_set_stmt");
  if (!arena_for_set_stmt)
    return;
  DBUG_PRINT("info", ("mem_root: %p  arena: %p",
                      arena_for_set_stmt->mem_root,
                      arena_for_set_stmt));
  arena_for_set_stmt->free_items();
  delete(arena_for_set_stmt);
  free_root(mem_root_for_set_stmt, MYF(MY_KEEP_PREALLOC));
  arena_for_set_stmt= 0;
  DBUG_VOID_RETURN;
}