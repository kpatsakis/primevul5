Status MutableGraphView::SwapRegularFaninsByPorts(absl::string_view node_name,
                                                  int from_port, int to_port) {
  auto error_status = [node_name, from_port, to_port](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', from_port=$1, to_port=$2",
                                     node_name, from_port, to_port);
    return MutationError("SwapRegularFaninsByPorts", params, msg);
  };

  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  const int last_regular_fanin_port =
      gtl::FindWithDefault(max_regular_input_port(), node, -1);
  TF_RETURN_IF_ERROR(CheckPortRange(from_port, /*min=*/0,
                                    last_regular_fanin_port, error_status));
  TF_RETURN_IF_ERROR(CheckPortRange(to_port, /*min=*/0, last_regular_fanin_port,
                                    error_status));

  if (from_port == to_port) {
    return Status::OK();
  }
  TensorId from_fanin = ParseTensorName(node->input(from_port));
  TensorId to_fanin = ParseTensorName(node->input(to_port));
  if (from_fanin == to_fanin) {
    return Status::OK();
  }

  InputPort from_input(node, from_port);
  InputPort to_input(node, to_port);
  NodeDef* from_fanin_node = GetNode(from_fanin.node());
  absl::flat_hash_set<InputPort>* from_fanouts =
      &fanouts()[{from_fanin_node, from_fanin.index()}];
  from_fanouts->erase(from_input);
  from_fanouts->insert(to_input);
  NodeDef* to_fanin_node = GetNode(to_fanin.node());
  absl::flat_hash_set<InputPort>* to_fanouts =
      &fanouts()[{to_fanin_node, to_fanin.index()}];
  to_fanouts->erase(to_input);
  to_fanouts->insert(from_input);

  node->mutable_input()->SwapElements(from_port, to_port);

  return Status::OK();
}