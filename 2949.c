void SwapControlledFanoutInputs(const MutableGraphView& graph,
                                const FanoutsMap::iterator& control_fanouts,
                                absl::string_view to_node_name) {
  absl::string_view from_node_name(control_fanouts->first.node->name());
  string control = TensorIdToString({to_node_name, Graph::kControlSlot});
  for (const auto& control_fanout : control_fanouts->second) {
    const int start = graph.NumFanins(*control_fanout.node,
                                      /*include_controlling_nodes=*/false);
    for (int i = start; i < control_fanout.node->input_size(); ++i) {
      TensorId tensor_id = ParseTensorName(control_fanout.node->input(i));
      if (tensor_id.node() == from_node_name) {
        control_fanout.node->set_input(i, control);
        break;
      }
    }
  }
}