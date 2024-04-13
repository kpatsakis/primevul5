Status GraphProperties::UpdateMerge(SymbolicShapeRefiner* shape_refiner,
                                    const NodeDef* node,
                                    bool* new_shapes) const {
  InferenceContext* ic = shape_refiner->GetContext(node);
  if (!ic) {
    // Now we can run shape inference
    TF_RETURN_IF_ERROR(shape_refiner->AddNode(node));
    ic = CHECK_NOTNULL(shape_refiner->GetContext(node));
    *new_shapes = true;

    // Infer the shape of the second output once and for all since it never
    // changes.
    ShapeHandle out1 = ic->Scalar();
    if (ic->num_outputs() >= 2) ic->set_output(1, out1);
  }

  ShapeHandle out;
  const std::vector<ShapeAndType>* out_handle = nullptr;
  bool out_initialized = false;
  for (const GraphView::Edge fanin : shape_refiner->graph().GetFaninEdges(
           *node, /*include_controlling_edges=*/false)) {
    InferenceContext* src_ic = shape_refiner->GetContext(fanin.src.node);
    if (!src_ic) {
      // Handling a loop for the first time, the back edge won't have any shape
      // info.
      continue;
    }
    ShapeHandle input = src_ic->output(fanin.src.port_id);
    ic->SetInput(fanin.dst.port_id, input);
    auto* input_handle =
        src_ic->output_handle_shapes_and_types(fanin.src.port_id);
    if (input_handle)
      ic->set_input_handle_shapes_and_types(fanin.dst.port_id, *input_handle);
    if (!out_initialized) {
      out_initialized = true;
      out = input;
      out_handle = input_handle;
    } else {
      // Note here only out, not out_handle, is modified.
      out = shape_refiner->OutputAsUnion(node, 0, input, out);
    }
  }

  if (*new_shapes || !shape_refiner->EquivalentShapes(out, ic->output(0))) {
    ic->set_output(0, out);
    if (out_handle) ic->set_output_handle_shapes_and_types(0, *out_handle);
    *new_shapes = true;
  }

  return Status::OK();
}