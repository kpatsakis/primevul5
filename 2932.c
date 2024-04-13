void MutableGraphView::UpdateMaxRegularOutputPortForRemovedFanin(
    const OutputPort& fanin,
    const absl::flat_hash_set<InputPort>& fanin_fanouts) {
  int max_port = max_regular_output_port()[fanin.node];
  if (!fanin_fanouts.empty() || max_port != fanin.port_id) {
    return;
  }
  bool updated_max_port = false;
  for (int i = fanin.port_id - 1; i >= 0; --i) {
    OutputPort fanin_port(fanin.node, i);
    if (!fanouts()[fanin_port].empty()) {
      max_regular_output_port()[fanin.node] = i;
      updated_max_port = true;
      break;
    }
  }
  if (!updated_max_port) {
    max_regular_output_port().erase(fanin.node);
  }
}