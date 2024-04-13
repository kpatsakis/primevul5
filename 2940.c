Status MutableGraphView::UpdateNode(
    absl::string_view node_name, absl::string_view op, absl::string_view device,
    absl::Span<const std::pair<string, AttrValue>> attrs) {
  auto error_status = [node_name, op, device, attrs](absl::string_view msg) {
    std::vector<string> attr_strs;
    attr_strs.reserve(attrs.size());
    for (const auto& attr : attrs) {
      string attr_str = absl::Substitute("('$0', $1)", attr.first,
                                         attr.second.ShortDebugString());
      attr_strs.push_back(attr_str);
    }
    string params =
        absl::Substitute("node_name='$0', op='$1', device='$2', attrs={$3}",
                         node_name, op, device, absl::StrJoin(attr_strs, ", "));
    return MutationError("UpdateNodeOp", params, msg);
  };

  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));

  MutableGraphView::OutputPort control_port(node, Graph::kControlSlot);
  auto control_fanouts = GetFanout(control_port);
  if (op == "Switch" && !control_fanouts.empty()) {
    return error_status(
        "can't change node op to Switch when node drives a control dependency "
        "(alternatively, we could add the identity node needed, but it seems "
        "like an unlikely event and probably a mistake)");
  }

  if (node->device() != device) {
    node->set_device(string(device));
  }
  node->mutable_attr()->clear();
  for (const auto& attr : attrs) {
    (*node->mutable_attr())[attr.first] = attr.second;
  }

  if (node->op() == op) {
    return Status::OK();
  }

  node->set_op(string(op));

  if (CanDedupControlWithRegularInput(*this, *node)) {
    for (const auto& control_fanout : control_fanouts) {
      if (HasRegularFaninNode(*this, *control_fanout.node, node->name())) {
        RemoveControllingFaninInternal(control_fanout.node, node);
      }
    }
  }

  return Status::OK();
}