Status CheckNodeExists(absl::string_view node_name, NodeDef* node,
                       ErrorHandler handler) {
  if (node == nullptr) {
    return handler(NodeMissingErrorMsg(node_name));
  }
  return Status::OK();
}