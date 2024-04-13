Status MutableGraphView::UpdateRegularFaninByPort(absl::string_view node_name,
                                                  int port,
                                                  const TensorId& fanin) {
  auto error_status = [node_name, port, fanin](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', port=$1, fanin='$2'",
                                     node_name, port, fanin.ToString());
    return MutationError("UpdateRegularFaninByPort", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckFaninIsRegular(fanin, error_status));
  TF_RETURN_IF_ERROR(CheckAddingFaninToSelf(node_name, fanin, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  const int last_regular_fanin_port =
      gtl::FindWithDefault(max_regular_input_port(), node, -1);
  TF_RETURN_IF_ERROR(
      CheckPortRange(port, /*min=*/0, last_regular_fanin_port, error_status));
  NodeDef* fanin_node = GetNode(fanin.node());
  TF_RETURN_IF_ERROR(CheckNodeExists(fanin.node(), fanin_node, error_status));

  TensorId tensor_id = ParseTensorName(node->input(port));
  if (tensor_id == fanin) {
    return Status::OK();
  }

  InputPort input(node, port);
  OutputPort from_fanin_port(nodes()[tensor_id.node()], tensor_id.index());
  absl::flat_hash_set<InputPort>* from_fanouts = &fanouts()[from_fanin_port];
  from_fanouts->erase(input);
  UpdateMaxRegularOutputPortForRemovedFanin(from_fanin_port, *from_fanouts);

  OutputPort to_fanin_port(fanin_node, fanin.index());
  fanouts()[to_fanin_port].insert(input);
  UpdateMaxRegularOutputPortForAddedFanin(to_fanin_port);

  node->set_input(port, TensorIdToString(fanin));

  if (CanDedupControlWithRegularInput(*this, *fanin_node)) {
    RemoveControllingFaninInternal(node, fanin_node);
  }

  return Status::OK();
}