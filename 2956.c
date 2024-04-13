void SwapFanoutInputs(const MutableGraphView& graph, FanoutsMap* fanouts,
                      MaxOutputPortsMap* max_output_ports, NodeDef* from_node,
                      NodeDef* to_node) {
  auto from_control_fanouts = fanouts->find({from_node, Graph::kControlSlot});
  if (from_control_fanouts != fanouts->end()) {
    SwapControlledFanoutInputs(graph, from_control_fanouts, to_node->name());
  }
  auto to_control_fanouts = fanouts->find({to_node, Graph::kControlSlot});
  if (to_control_fanouts != fanouts->end()) {
    SwapControlledFanoutInputs(graph, to_control_fanouts, from_node->name());
  }
  auto from_max_port = max_output_ports->find(from_node);
  if (from_max_port != max_output_ports->end()) {
    SwapRegularFanoutInputs(fanouts, from_node, to_node->name(),
                            from_max_port->second);
  }
  auto to_max_port = max_output_ports->find(to_node);
  if (to_max_port != max_output_ports->end()) {
    SwapRegularFanoutInputs(fanouts, to_node, from_node->name(),
                            to_max_port->second);
  }
}