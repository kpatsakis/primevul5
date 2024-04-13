bool IsOutputPortControlling(const MutableGraphView::OutputPort& port) {
  return port.port_id == Graph::kControlSlot;
}