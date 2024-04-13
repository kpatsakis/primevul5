bool HasRegularFaninNode(const MutableGraphView& graph, const NodeDef& node,
                         absl::string_view fanin_node_name) {
  const int num_regular_fanins =
      graph.NumFanins(node, /*include_controlling_nodes=*/false);
  for (int i = 0; i < num_regular_fanins; ++i) {
    if (ParseTensorName(node.input(i)).node() == fanin_node_name) {
      return true;
    }
  }
  return false;
}