Status VerboseShapeInferenceLogging(const GraphDef& graph_def,
                                    SymbolicShapeRefiner* refiner,
                                    SymbolicShapeManager* shape_manager) {
  // As logging all the nodes would generate too many lines, we by default
  // skip this detailed logging. Users may add nodes of interest to
  // node_names_for_logging to enable detailed logging.
  absl::flat_hash_set<std::string> node_names_for_logging = {};
  if (!VLOG_IS_ON(3) || node_names_for_logging.empty()) {
    return Status::OK();
  }

  auto should_log = [&node_names_for_logging](std::string node_name) {
    return node_names_for_logging.find(node_name) !=
           node_names_for_logging.end();
  };

  for (const NodeDef& node : graph_def.node()) {
    if (!should_log(node.name())) {
      continue;
    }
    auto ctx = refiner->GetNodeContext(&node);
    if (!ctx) {
      continue;
    }
    auto* ic = ctx->inference_context.get();
    VLOG(3) << "Shape inference for node : " << node.name();
    VLOG(3) << ctx->DebugString(node);
    std::string merged_shapes = "Merged shapes from SymbolicShapManager:\n";
    for (int i = 0; i < ic->num_inputs(); ++i) {
      absl::StrAppend(
          &merged_shapes, " input[", i, "] -- ",
          ic->DebugString(shape_manager->GetMergedShape(ic, ic->input(i))),
          "\n");
    }
    for (int i = 0; i < ic->num_outputs(); ++i) {
      absl::StrAppend(
          &merged_shapes, " output[", i, "] -- ",
          ic->DebugString(shape_manager->GetMergedShape(ic, ic->output(i))),
          "\n");
    }
    VLOG(3) << merged_shapes;
    VLOG(3) << "--------------------------------";
    VLOG(3) << "";
  }

  return Status::OK();
}