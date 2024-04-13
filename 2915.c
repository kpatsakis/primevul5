Status MutableGraphView::RemoveControllingFanin(
    absl::string_view node_name, absl::string_view fanin_node_name) {
  auto error_status = [node_name, fanin_node_name](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', fanin_node_name='$1'",
                                     node_name, fanin_node_name);
    return MutationError("RemoveControllingFanin", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckRemovingFaninFromSelf(
      node_name, {fanin_node_name, Graph::kControlSlot}, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  NodeDef* fanin_node = GetNode(fanin_node_name);
  TF_RETURN_IF_ERROR(
      CheckNodeExists(fanin_node_name, fanin_node, error_status));

  RemoveControllingFaninInternal(node, fanin_node);
  return Status::OK();
}