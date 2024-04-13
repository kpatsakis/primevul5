void SwapRegularFanoutsAndMaxPortValues(FanoutsMap* fanouts,
                                        MaxOutputPortsMap* max_output_ports,
                                        NodeDef* from_node, NodeDef* to_node) {
  auto from_max_port = max_output_ports->find(from_node);
  auto to_max_port = max_output_ports->find(to_node);
  bool from_exists = from_max_port != max_output_ports->end();
  bool to_exists = to_max_port != max_output_ports->end();

  auto forward_fanouts = [fanouts](NodeDef* from, NodeDef* to, int start,
                                   int end) {
    for (int i = start; i <= end; ++i) {
      MutableGraphView::OutputPort from_port(from, i);
      auto from_fanouts = fanouts->find(from_port);
      if (from_fanouts != fanouts->end()) {
        MutableGraphView::OutputPort to_port(to, i);
        fanouts->emplace(to_port, std::move(from_fanouts->second));
        fanouts->erase(from_port);
      }
    }
  };

  if (from_exists && to_exists) {
    const int from = from_max_port->second;
    const int to = to_max_port->second;
    const int shared = std::min(from, to);
    for (int i = 0; i <= shared; ++i) {
      MutableGraphView::OutputPort from_port(from_node, i);
      auto from_fanouts = fanouts->find(from_port);
      MutableGraphView::OutputPort to_port(to_node, i);
      auto to_fanouts = fanouts->find(to_port);
      SwapFanoutsMapValues(fanouts, from_port, from_fanouts, to_port,
                           to_fanouts);
    }
    if (to > from) {
      forward_fanouts(to_node, from_node, shared + 1, to);
    } else if (from > to) {
      forward_fanouts(from_node, to_node, shared + 1, from);
    }

    std::swap(from_max_port->second, to_max_port->second);
  } else if (from_exists) {
    forward_fanouts(from_node, to_node, 0, from_max_port->second);

    max_output_ports->emplace(to_node, from_max_port->second);
    max_output_ports->erase(from_node);
  } else if (to_exists) {
    forward_fanouts(to_node, from_node, 0, to_max_port->second);

    max_output_ports->emplace(from_node, to_max_port->second);
    max_output_ports->erase(to_node);
  }
}