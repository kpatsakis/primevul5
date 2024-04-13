Status MutableGraphView::RemoveAllFanins(absl::string_view node_name,
                                         bool keep_controlling_fanins) {
  NodeDef* node = GetNode(node_name);
  if (node == nullptr) {
    string params =
        absl::Substitute("node_name='$0', keep_controlling_fanins=$1",
                         node_name, keep_controlling_fanins);
    return MutationError("RemoveAllFanins", params,
                         NodeMissingErrorMsg(node_name));
  }

  if (node->input().empty()) {
    return Status::OK();
  }

  const int num_regular_fanins =
      NumFanins(*node, /*include_controlling_nodes=*/false);
  RemoveFaninsInternal(node, keep_controlling_fanins);
  if (keep_controlling_fanins) {
    if (num_regular_fanins == 0) {
      return Status::OK();
    } else if (num_regular_fanins < node->input_size()) {
      node->mutable_input()->DeleteSubrange(0, num_regular_fanins);
    } else {
      node->clear_input();
    }
  } else {
    node->clear_input();
  }
  return Status::OK();
}