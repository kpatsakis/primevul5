Status MutableGraphView::RemoveRegularFaninByPort(absl::string_view node_name,
                                                  int port) {
  auto error_status = [node_name, port](absl::string_view msg) {
    string params =
        absl::Substitute("node_name='$0', port=$1", node_name, port);
    return MutationError("RemoveRegularFaninByPort", params, msg);
  };

  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  const int last_regular_fanin_port =
      gtl::FindWithDefault(max_regular_input_port(), node, -1);
  TF_RETURN_IF_ERROR(
      CheckPortRange(port, /*min=*/0, last_regular_fanin_port, error_status));

  TensorId tensor_id = ParseTensorName(node->input(port));
  OutputPort fanin_port(nodes()[tensor_id.node()], tensor_id.index());
  fanouts()[fanin_port].erase({node, port});
  auto mutable_inputs = node->mutable_input();
  for (int i = port + 1; i <= last_regular_fanin_port; ++i) {
    TensorId tensor_id = ParseTensorName(node->input(i));
    OutputPort fanin_port(nodes()[tensor_id.node()], tensor_id.index());
    absl::flat_hash_set<InputPort>* fanouts_set = &fanouts()[fanin_port];
    fanouts_set->erase({node, i});
    fanouts_set->insert({node, i - 1});
    mutable_inputs->SwapElements(i - 1, i);
  }
  const int last_node_input = node->input_size() - 1;
  if (last_regular_fanin_port < last_node_input) {
    mutable_inputs->SwapElements(last_regular_fanin_port, last_node_input);
  }
  mutable_inputs->RemoveLast();

  const int updated_last_regular_input_port = last_regular_fanin_port - 1;
  if (updated_last_regular_input_port < 0) {
    max_regular_input_port().erase(node);
  } else {
    max_regular_input_port()[node] = updated_last_regular_input_port;
  }

  return Status::OK();
}