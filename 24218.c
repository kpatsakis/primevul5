bool No_such_table_error_handler::safely_trapped_errors()
{
  /*
    If m_unhandled_errors != 0, something else, unanticipated, happened,
    so the error is not trapped but returned to the caller.
    Multiple ER_NO_SUCH_TABLE can be raised in case of views.
  */
  return ((m_handled_errors > 0) && (m_unhandled_errors == 0));
}