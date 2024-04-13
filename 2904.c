Status MutableGraphView::UpdateFanouts(absl::string_view from_node_name,
                                       absl::string_view to_node_name) {
  NodeDef* from_node = GetNode(from_node_name);
  TF_RETURN_IF_ERROR(
      CheckNodeExists(from_node_name, from_node,
                      UpdateFanoutsError(from_node_name, to_node_name)));
  NodeDef* to_node = GetNode(to_node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(
      to_node_name, to_node, UpdateFanoutsError(from_node_name, to_node_name)));

  return UpdateFanoutsInternal(from_node, to_node);
}