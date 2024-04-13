Status MutableGraphView::AddControllingFanin(absl::string_view node_name,
                                             const TensorId& fanin) {
  auto error_status = [node_name, fanin](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', fanin='$1'", node_name,
                                     fanin.ToString());
    return MutationError("AddControllingFanin", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckFaninIsValid(fanin, error_status));
  TF_RETURN_IF_ERROR(CheckAddingFaninToSelf(node_name, fanin, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  NodeDef* fanin_node = GetNode(fanin.node());
  TF_RETURN_IF_ERROR(CheckNodeExists(fanin.node(), fanin_node, error_status));

  OutputPort fanin_port(fanin_node, fanin.index());

  string error_msg = "";
  NodeDef* control_node = GetControllingFaninToAdd(
      node_name, {fanin_node, fanin.index()}, &error_msg);
  if (!error_msg.empty()) {
    return error_status(error_msg);
  }
  if (control_node == nullptr) {
    control_node = GetOrCreateIdentityConsumingSwitch(fanin_port);
  }
  AddFaninInternal(node, {control_node, Graph::kControlSlot});

  return Status::OK();
}