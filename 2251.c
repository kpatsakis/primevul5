Status ValidateSymbolicShapeManager(const GraphDef& graph_def,
                                    SymbolicShapeRefiner* refiner,
                                    SymbolicShapeManager* shape_manager) {
  if (!VLOG_IS_ON(1)) {
    return Status::OK();
  }

  VLOG(1) << "Checking any conflics in shapes and dimensions ...";
  int64_t num_incompatible_shapes = 0;
  for (const NodeDef& node : graph_def.node()) {
    auto ctx = refiner->GetNodeContext(&node);
    if (!ctx) {
      continue;
    }
    auto* ic = ctx->inference_context.get();
    for (int i = 0; i < ic->num_inputs(); ++i) {
      const auto& shape = ic->input(i);
      const auto& merged_shape = shape_manager->GetMergedShape(ic, shape);
      if (!refiner->CompatibleShapes(shape, merged_shape)) {
        num_incompatible_shapes++;
        VLOG(1) << "**** Incompatible shape from SymbolicShapeManager "
                << "for node " << node.name() << " input (" << i << ") "
                << ic->DebugString(shape)
                << " vs. merged: " << ic->DebugString(merged_shape);
      }
    }
    for (int i = 0; i < ic->num_outputs(); ++i) {
      const auto& shape = ic->output(i);
      const auto& merged_shape = shape_manager->GetMergedShape(ic, shape);
      if (!refiner->CompatibleShapes(shape, merged_shape)) {
        num_incompatible_shapes++;
        VLOG(1) << "**** Incompatible shape from SymbolicShapeManager "
                << "for node " << node.name() << " output (" << i << ") "
                << ic->DebugString(shape)
                << " vs. merged: " << ic->DebugString(merged_shape);
      }
    }
  }
  if (num_incompatible_shapes > 0) {
    VLOG(1) << "**** WARNING: " << num_incompatible_shapes
            << " incompatible shapes from SymbolicShapeManager.";
  } else {
    VLOG(1) << "**** No incompatible shape found from SymbolicShapeManager.";
  }

  return Status::OK();
}