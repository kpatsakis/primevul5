static bool inject_reprepare(THD *thd)
{
  if (thd->m_reprepare_observer && thd->stmt_arena->is_reprepared == FALSE)
  {
    thd->m_reprepare_observer->report_error(thd);
    return TRUE;
  }

  return FALSE;
}