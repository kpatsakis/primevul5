bool MutableGraphView::AddFaninInternal(NodeDef* node,
                                        const OutputPort& fanin) {
  int num_regular_fanins =
      NumFanins(*node, /*include_controlling_nodes=*/false);
  bool input_is_control = IsOutputPortControlling(fanin);
  bool can_dedup_control_with_regular_input =
      CanDedupControlWithRegularInput(*this, *fanin.node);
  // Don't add duplicate control dependencies.
  if (input_is_control) {
    const int start =
        can_dedup_control_with_regular_input ? 0 : num_regular_fanins;
    for (int i = start; i < node->input_size(); ++i) {
      if (ParseTensorName(node->input(i)).node() == fanin.node->name()) {
        return false;
      }
    }
  }

  InputPort input;
  input.node = node;
  input.port_id = input_is_control ? Graph::kControlSlot : num_regular_fanins;

  node->add_input(TensorIdToString({fanin.node->name(), fanin.port_id}));
  if (!input_is_control) {
    const int last_node_input = node->input_size() - 1;
    // If there are control dependencies in node, move newly inserted fanin to
    // be before such control dependencies.
    if (num_regular_fanins < last_node_input) {
      node->mutable_input()->SwapElements(last_node_input, num_regular_fanins);
    }
  }

  fanouts()[fanin].insert(input);
  if (max_regular_output_port()[fanin.node] < fanin.port_id) {
    max_regular_output_port()[fanin.node] = fanin.port_id;
  }

  // Update max input port and dedup control dependencies.
  if (!input_is_control) {
    max_regular_input_port()[node] = num_regular_fanins;
    if (can_dedup_control_with_regular_input) {
      RemoveControllingFaninInternal(node, fanin.node);
    }
  }

  return true;
}