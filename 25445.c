  void set_geometry_type(uint type)
  {
    DBUG_ASSERT(type <= m_geometry_type_unknown);
    m_geometry_type= type;
  }