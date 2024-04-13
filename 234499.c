bool skip_setup_conds(THD *thd)
{
  return (!thd->stmt_arena->is_conventional()
          && !thd->stmt_arena->is_stmt_prepare_or_first_sp_execute())
         || thd->lex->is_view_context_analysis();
}