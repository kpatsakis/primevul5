  bool safely_trapped_errors()
  {
    /*
      Check for m_handled_errors is here for extra safety.
      It can be useful in situation when call to open_table()
      fails because some error which was suppressed by another
      error handler (e.g. in case of MDL deadlock which we
      decided to solve by back-off and retry).
    */
    return (m_handled_errors && (! m_unhandled_errors));
  }