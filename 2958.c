Status MutableGraphView::UpdateNodeName(absl::string_view from_node_name,
                                        absl::string_view to_node_name,
                                        bool update_fanouts) {
  auto error_status = [from_node_name, to_node_name,
                       update_fanouts](absl::string_view msg) {
    string params = absl::Substitute(
        "from_node_name='$0', to_node_name='$1', update_fanouts=$2",
        from_node_name, to_node_name, update_fanouts);
    return MutationError("UpdateNodeName", params, msg);
  };

  NodeDef* node = GetNode(from_node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(from_node_name, node, error_status));

  if (node->name() == to_node_name) {
    return Status::OK();
  }
  if (HasNode(to_node_name)) {
    return error_status(
        "can't update node name because new node name is in use");
  }
  auto max_output_port = max_regular_output_port().find(node);
  const bool has_max_output_port =
      max_output_port != max_regular_output_port().end();
  auto control_fanouts = fanouts().find({node, Graph::kControlSlot});

  if (update_fanouts) {
    SwapControlledFanoutInputs(*this, control_fanouts, to_node_name);
    if (has_max_output_port) {
      SwapRegularFanoutInputs(&fanouts(), node, to_node_name,
                              max_output_port->second);
    }
  } else if (has_max_output_port ||
             HasFanoutValue(fanouts(), control_fanouts)) {
    return error_status("can't update node name because node has fanouts");
  }

  nodes().erase(node->name());
  node->set_name(string(to_node_name));
  nodes().emplace(node->name(), node);
  return Status::OK();
}