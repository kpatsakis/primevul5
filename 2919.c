bool CanDedupControlWithRegularInput(const MutableGraphView& graph,
                                     absl::string_view control_node_name) {
  NodeDef* control_node = graph.GetNode(control_node_name);
  if (control_node == nullptr) {
    return false;
  }
  return CanDedupControlWithRegularInput(graph, *control_node);
}