void MutableGraphView::UpdateMaxRegularOutputPortForAddedFanin(
    const OutputPort& fanin) {
  if (max_regular_output_port()[fanin.node] < fanin.port_id) {
    max_regular_output_port()[fanin.node] = fanin.port_id;
  }
}