  DimensionHandle GetUnknownOutputDim(const NodeDef* node, int index,
                                      int dim_id) {
    DimId id{node, index, dim_id};
    auto it = unknown_dims_.find(id);
    if (it != unknown_dims_.end()) {
      return it->second;
    }
    InferenceContext* c = GetContext(node);
    DimensionHandle dim = c->UnknownDim();
    unknown_dims_[id] = dim;
    return dim;
  }