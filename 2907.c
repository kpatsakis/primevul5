Status MutableGraphView::UpdateFanin(absl::string_view node_name,
                                     const TensorId& from_fanin,
                                     const TensorId& to_fanin) {
  auto error_status = [node_name, from_fanin, to_fanin](absl::string_view msg) {
    string params =
        absl::Substitute("node_name='$0', from_fanin='$1', to_fanin='$2'",
                         node_name, from_fanin.ToString(), to_fanin.ToString());
    return MutationError("UpdateFanin", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckFaninIsValid(from_fanin, error_status));
  TF_RETURN_IF_ERROR(CheckFaninIsValid(to_fanin, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  NodeDef* from_fanin_node = GetNode(from_fanin.node());
  TF_RETURN_IF_ERROR(
      CheckNodeExists(from_fanin.node(), from_fanin_node, error_status));
  NodeDef* to_fanin_node = GetNode(to_fanin.node());
  TF_RETURN_IF_ERROR(
      CheckNodeExists(to_fanin.node(), to_fanin_node, error_status));

  // When replacing a non control dependency fanin with a control dependency, or
  // vice versa, remove and add, so ports can be updated properly in fanout(s).
  bool to_fanin_is_control = IsTensorIdControlling(to_fanin);
  if (to_fanin_is_control && IsSwitch(*to_fanin_node)) {
    // Can't add Switch node as a control dependency.
    return error_status(
        absl::Substitute("can't update to fanin '$0' as it will become a "
                         "Switch control dependency",
                         to_fanin.ToString()));
  }
  if (node_name == from_fanin.node() || node_name == to_fanin.node()) {
    return error_status("can't update fanin to or from self");
  }

  if (from_fanin == to_fanin) {
    return Status::OK();
  }

  bool from_fanin_is_control = IsTensorIdControlling(from_fanin);
  if (from_fanin_is_control || to_fanin_is_control) {
    bool modified = false;
    if (from_fanin_is_control) {
      modified |= RemoveControllingFaninInternal(node, from_fanin_node);
    } else {
      modified |= RemoveRegularFaninInternal(
          node, {from_fanin_node, from_fanin.index()});
    }
    if (modified) {
      AddFaninInternal(node, {to_fanin_node, to_fanin.index()});
    }
    return Status::OK();
  }

  // In place mutation of regular fanins, requires no shifting of ports.
  string to_fanin_string = TensorIdToString(to_fanin);
  const int num_regular_fanins =
      NumFanins(*node, /*include_controlling_nodes=*/false);
  bool modified = false;
  for (int i = 0; i < num_regular_fanins; ++i) {
    if (ParseTensorName(node->input(i)) == from_fanin) {
      InputPort input(node, i);

      OutputPort from_fanin_port(from_fanin_node, from_fanin.index());
      fanouts()[from_fanin_port].erase(input);

      OutputPort to_fanin_port(to_fanin_node, to_fanin.index());
      fanouts()[to_fanin_port].insert(input);

      node->set_input(i, to_fanin_string);
      modified = true;
    }
  }

  // Dedup control dependencies and update max regular output ports.
  if (modified) {
    OutputPort from_fanin_port(from_fanin_node, from_fanin.index());
    UpdateMaxRegularOutputPortForRemovedFanin(
        {from_fanin_node, from_fanin.index()}, fanouts()[from_fanin_port]);
    if (max_regular_output_port()[to_fanin_node] < to_fanin.index()) {
      max_regular_output_port()[to_fanin_node] = to_fanin.index();
    }
    if (CanDedupControlWithRegularInput(*this, *to_fanin_node)) {
      RemoveControllingFaninInternal(node, to_fanin_node);
    }
  }

  return Status::OK();
}