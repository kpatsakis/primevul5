bool LEX::set_arena_for_set_stmt(Query_arena *backup)
{
  DBUG_ENTER("LEX::set_arena_for_set_stmt");
  DBUG_ASSERT(arena_for_set_stmt== 0);
  if (!mem_root_for_set_stmt)
  {
    mem_root_for_set_stmt= new MEM_ROOT();
    if (!(mem_root_for_set_stmt))
      DBUG_RETURN(1);
    init_sql_alloc(mem_root_for_set_stmt, ALLOC_ROOT_SET, ALLOC_ROOT_SET,
                   MYF(MY_THREAD_SPECIFIC));
  }
  if (!(arena_for_set_stmt= new(mem_root_for_set_stmt)
        Query_arena_memroot(mem_root_for_set_stmt,
                            Query_arena::STMT_INITIALIZED)))
    DBUG_RETURN(1);
  DBUG_PRINT("info", ("mem_root: %p  arena: %p",
                      mem_root_for_set_stmt,
                      arena_for_set_stmt));
  thd->set_n_backup_active_arena(arena_for_set_stmt, backup);
  DBUG_RETURN(0);
}