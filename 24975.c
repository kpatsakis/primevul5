  void join(const Item *item)
  {
    // Ignore implicit NULLs
    if (m_geometry_type == m_geometry_type_unknown)
      copy(item->type_handler(), item);
    else if (item->type_handler() == &type_handler_geometry)
    {
      m_geometry_type=
        Field_geom::geometry_type_merge((Field_geom::geometry_type)
                                         m_geometry_type,
                                        (Field_geom::geometry_type)
                                         item->uint_geometry_type());
    }
  }