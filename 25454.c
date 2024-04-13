  Type_geometry_attributes(const Type_handler *handler,
                           const Type_all_attributes *gattr)
   :m_geometry_type(m_geometry_type_unknown)
  {
    copy(handler, gattr);
  }