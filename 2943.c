void SwapRegularFanoutInputs(FanoutsMap* fanouts, NodeDef* from_node,
                             absl::string_view to_node_name, int max_port) {
  MutableGraphView::OutputPort port;
  port.node = from_node;
  for (int i = 0; i <= max_port; ++i) {
    port.port_id = i;
    auto it = fanouts->find(port);
    if (it == fanouts->end()) {
      continue;
    }
    string input = TensorIdToString({to_node_name, i});
    for (const auto& fanout : it->second) {
      fanout.node->set_input(fanout.port_id, input);
    }
  }
}