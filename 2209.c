  explicit SymbolicShapeRefiner(
      const GraphView& graph,
      const absl::flat_hash_map<string, absl::flat_hash_set<int>>& fed_ports,
      const bool aggressive_shape_inference)
      : graph_(graph),
        function_library_(OpRegistry::Global(), graph.graph()->library()),
        fed_ports_(fed_ports),
        aggressive_shape_inference_(aggressive_shape_inference) {
    graph_def_version_ = graph.graph()->versions().producer();
    node_to_context_.reserve(graph.graph()->node_size());
  }