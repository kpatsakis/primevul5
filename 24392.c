bool LEX::restore_set_statement_var()
{
  bool err= false;
  DBUG_ENTER("LEX::restore_set_statement_var");
  if (!old_var_list.is_empty())
  {
    DBUG_PRINT("info", ("vars: %d", old_var_list.elements));
    err= sql_set_variables(thd, &old_var_list, false);
    old_var_list.empty();
    free_arena_for_set_stmt();
  }
  DBUG_ASSERT(!is_arena_for_set_stmt());
  DBUG_RETURN(err);
}