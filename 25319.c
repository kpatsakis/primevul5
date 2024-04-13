  Field::geometry_type get_geometry_type() const
  {
    return m_geometry_type == m_geometry_type_unknown ?
           Field::GEOM_GEOMETRY :
           (Field::geometry_type) m_geometry_type;
  }