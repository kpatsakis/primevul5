Status MutableGraphView::AddRegularFanin(absl::string_view node_name,
                                         const TensorId& fanin) {
  auto error_status = [node_name, fanin](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0', fanin='$1'", node_name,
                                     fanin.ToString());
    return MutationError("AddRegularFanin", params, msg);
  };

  TF_RETURN_IF_ERROR(CheckFaninIsRegular(fanin, error_status));
  TF_RETURN_IF_ERROR(CheckAddingFaninToSelf(node_name, fanin, error_status));
  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));
  NodeDef* fanin_node = GetNode(fanin.node());
  TF_RETURN_IF_ERROR(CheckNodeExists(fanin.node(), fanin_node, error_status));

  AddFaninInternal(node, {fanin_node, fanin.index()});
  return Status::OK();
}