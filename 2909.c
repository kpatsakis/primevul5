Status MutableGraphView::CheckNodesCanBeDeleted(
    const absl::flat_hash_set<string>& nodes_to_delete) {
  std::vector<string> missing_nodes;
  std::vector<string> nodes_with_fanouts;
  for (const string& node_name_to_delete : nodes_to_delete) {
    NodeDef* node = GetNode(node_name_to_delete);
    if (node == nullptr) {
      // Can't delete missing node.
      missing_nodes.push_back(node_name_to_delete);
      continue;
    }
    const int max_port = gtl::FindWithDefault(max_regular_output_port(), node,
                                              Graph::kControlSlot);
    for (int i = Graph::kControlSlot; i <= max_port; ++i) {
      auto it = fanouts().find({node, i});
      bool has_retained_fanout = false;
      if (it != fanouts().end()) {
        for (const auto& fanout : it->second) {
          // Check if fanouts are of nodes to be deleted, and if so, they can be
          // ignored, as they will be removed also.
          if (!nodes_to_delete.contains(fanout.node->name())) {
            // Removing node will leave graph in an invalid state.
            has_retained_fanout = true;
            break;
          }
        }
      }
      if (has_retained_fanout) {
        nodes_with_fanouts.push_back(node_name_to_delete);
        break;
      }
    }
  }

  // Error message can get quite long, so we only show the first 5 node names.
  auto sort_and_sample = [](std::vector<string>* s) {
    constexpr int kMaxNodeNames = 5;
    std::sort(s->begin(), s->end());
    if (s->size() > kMaxNodeNames) {
      return absl::StrCat(
          absl::StrJoin(s->begin(), s->begin() + kMaxNodeNames, ", "), ", ...");
    }
    return absl::StrJoin(*s, ", ");
  };

  if (!missing_nodes.empty()) {
    VLOG(2) << absl::Substitute("Attempting to delete missing node(s) [$0].",
                                sort_and_sample(&missing_nodes));
  }
  if (!nodes_with_fanouts.empty()) {
    std::vector<string> input_node_names(nodes_to_delete.begin(),
                                         nodes_to_delete.end());
    string params = absl::Substitute("nodes_to_delete={$0}",
                                     sort_and_sample(&input_node_names));
    string error_msg =
        absl::Substitute("can't delete node(s) with retained fanouts(s) [$0]",
                         sort_and_sample(&nodes_with_fanouts));
    return MutationError("DeleteNodes", params, error_msg);
  }

  return Status::OK();
}