Open_table_context::Open_table_context(THD *thd, uint flags)
  :m_thd(thd),
   m_failed_table(NULL),
   m_start_of_statement_svp(thd->mdl_context.mdl_savepoint()),
   m_timeout(flags & MYSQL_LOCK_IGNORE_TIMEOUT ?
             LONG_TIMEOUT : thd->variables.lock_wait_timeout),
   m_flags(flags),
   m_action(OT_NO_ACTION),
   m_has_locks(thd->mdl_context.has_locks()),
   m_has_protection_against_grl(0)
{}