  bool handle_condition(THD *thd,
                        uint sql_errno,
                        const char* sqlstate,
                        Sql_condition::enum_warning_level *level,
                        const char* msg,
                        Sql_condition ** cond_hdl)
  {
    *cond_hdl= NULL;
    if (sql_errno == ER_OPEN_AS_READONLY)
    {
      handled_errors++;
      return TRUE;
    }
    if (*level == Sql_condition::WARN_LEVEL_ERROR)
      unhandled_errors++;
    return FALSE;
  }