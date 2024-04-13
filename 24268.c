Repair_mrg_table_error_handler::handle_condition(THD *,
                                                 uint sql_errno,
                                                 const char*,
                                                 Sql_condition::enum_warning_level *level,
                                                 const char*,
                                                 Sql_condition ** cond_hdl)
{
  *cond_hdl= NULL;
  if (sql_errno == ER_NO_SUCH_TABLE ||
      sql_errno == ER_NO_SUCH_TABLE_IN_ENGINE ||
      sql_errno == ER_WRONG_MRG_TABLE)
  {
    m_handled_errors= true;
    return TRUE;
  }

  m_unhandled_errors= true;
  return FALSE;
}