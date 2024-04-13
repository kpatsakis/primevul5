void st_select_lex_unit::set_limit(st_select_lex *sl)
{
  DBUG_ASSERT(!thd->stmt_arena->is_stmt_prepare());

  offset_limit_cnt= sl->get_offset();
  select_limit_cnt= sl->get_limit();
  if (select_limit_cnt + offset_limit_cnt >= select_limit_cnt)
    select_limit_cnt+= offset_limit_cnt;
  else
    select_limit_cnt= HA_POS_ERROR;
}