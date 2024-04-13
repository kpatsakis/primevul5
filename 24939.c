  void set(size_t pos, size_t length)
  {
    DBUG_ASSERT(pos < UINT_MAX32);
    DBUG_ASSERT(length < UINT_MAX32);
    m_pos= (uint) pos;
    m_length= (uint) length;
  }