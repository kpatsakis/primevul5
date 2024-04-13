  NodeContext* GetNodeContext(const NodeDef* node) {
    auto it = node_to_context_.find(node);
    if (it == node_to_context_.end()) {
      return nullptr;
    }
    return &it->second;
  }