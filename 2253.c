    std::size_t operator()(const ShapeId& shp) const {
      return std::hash<const NodeDef*>{}(shp.node) + shp.port_id;
    }