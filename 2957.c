Status MutableGraphView::AddSubgraph(GraphDef&& subgraph) {
  // 1. Add all new functions and check that functions with the same name
  // have identical definition.
  const int function_size = subgraph.library().function_size();
  if (function_size > 0) {
    absl::flat_hash_map<absl::string_view, const FunctionDef*> graph_fdefs;
    for (const FunctionDef& fdef : graph()->library().function()) {
      graph_fdefs.emplace(fdef.signature().name(), &fdef);
    }

    for (FunctionDef& fdef : *subgraph.mutable_library()->mutable_function()) {
      const auto graph_fdef = graph_fdefs.find(fdef.signature().name());

      if (graph_fdef == graph_fdefs.end()) {
        VLOG(3) << "Add new function definition: " << fdef.signature().name();
        graph()->mutable_library()->add_function()->Swap(&fdef);
      } else {
        if (!FunctionDefsEqual(fdef, *graph_fdef->second)) {
          return MutationError(
              "AddSubgraph",
              absl::Substitute("function_size=$0", function_size),
              absl::StrCat(
                  "Found different function definition with the same name: ",
                  fdef.signature().name()));
        }
      }
    }
  }

  // 2. Add all nodes to the underlying graph.
  int node_size_before = graph()->node_size();

  for (NodeDef& node : *subgraph.mutable_node()) {
    auto* node_in_graph = graph()->add_node();
    node_in_graph->Swap(&node);
    TF_RETURN_IF_ERROR(AddUniqueNode(node_in_graph));
  }

  // TODO(ezhulenev, lyandy): Right now AddAndDedupFanouts do not check that
  // fanins actually exists in the graph, and there is already TODO for that.

  for (int i = node_size_before; i < graph()->node_size(); ++i) {
    NodeDef* node = graph()->mutable_node(i);
    AddAndDedupFanouts(node);
  }

  return Status::OK();
}