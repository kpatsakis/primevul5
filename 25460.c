  void copy(const Type_handler *handler, const Type_all_attributes *gattr)
  {
    // Ignore implicit NULLs
    m_geometry_type= handler == &type_handler_geometry ?
                     gattr->uint_geometry_type() :
                     m_geometry_type_unknown;
  }