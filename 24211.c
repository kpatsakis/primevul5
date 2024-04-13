bool MDL_deadlock_handler::handle_condition(THD *,
                                            uint sql_errno,
                                            const char*,
                                            Sql_condition::enum_warning_level*,
                                            const char*,
                                            Sql_condition ** cond_hdl)
{
  *cond_hdl= NULL;
  if (! m_is_active && sql_errno == ER_LOCK_DEADLOCK)
  {
    /* Disable the handler to avoid infinite recursion. */
    m_is_active= TRUE;
    (void) m_ot_ctx->request_backoff_action(
             Open_table_context::OT_BACKOFF_AND_RETRY,
             NULL);
    m_is_active= FALSE;
    /*
      If the above back-off request failed, a new instance of
      ER_LOCK_DEADLOCK error was emitted. Thus the current
      instance of error condition can be treated as handled.
    */
    return TRUE;
  }
  return FALSE;
}