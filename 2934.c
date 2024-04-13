Status MutableGraphView::AddRegularFaninByPort(absl::string_view node_name,
                                               int port,
                                               const TensorId& fanin) {
  auto error_status = [node_name, port, fanin](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', port=$1, fanin='$2'",
                                     node_name, port, fanin.ToString());
    return MutationError("AddRegularFaninByPort", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckFaninIsRegular(fanin, error_status));
  TF_RETURN_IF_ERROR(CheckAddingFaninToSelf(node_name, fanin, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  const int num_regular_fanins =
      NumFanins(*node, /*include_controlling_nodes=*/false);
  TF_RETURN_IF_ERROR(
      CheckPortRange(port, /*min=*/0, num_regular_fanins, error_status));
  NodeDef* fanin_node = GetNode(fanin.node());
  TF_RETURN_IF_ERROR(CheckNodeExists(fanin.node(), fanin_node, error_status));

  const int last_node_input = node->input_size();
  node->add_input(TensorIdToString(fanin));
  node->mutable_input()->SwapElements(num_regular_fanins, last_node_input);
  for (int i = num_regular_fanins - 1; i >= port; --i) {
    TensorId tensor_id = ParseTensorName(node->input(i));
    OutputPort fanin_port(nodes()[tensor_id.node()], tensor_id.index());
    absl::flat_hash_set<InputPort>* fanouts_set = &fanouts()[fanin_port];
    fanouts_set->erase({node, i});
    fanouts_set->insert({node, i + 1});
    node->mutable_input()->SwapElements(i, i + 1);
  }

  OutputPort fanin_port(fanin_node, fanin.index());
  fanouts()[fanin_port].insert({node, port});
  UpdateMaxRegularOutputPortForAddedFanin(fanin_port);

  max_regular_input_port()[node] = num_regular_fanins;
  if (CanDedupControlWithRegularInput(*this, *fanin_node)) {
    RemoveControllingFaninInternal(node, fanin_node);
  }

  return Status::OK();
}