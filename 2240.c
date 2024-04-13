Status GraphProperties::UpdateEnqueue(
    const NodeDef* enqueue_node,
    const absl::flat_hash_map<const NodeDef*, const NodeDef*>& resource_handles,
    SymbolicShapeRefiner* shape_refiner, bool* new_shapes) {
  auto ctx = shape_refiner->GetNodeContext(enqueue_node);
  if (!ctx) {
    TF_RETURN_IF_ERROR(shape_refiner->AddNode(enqueue_node));
    ctx = CHECK_NOTNULL(shape_refiner->GetNodeContext(enqueue_node));
  }

  auto it = resource_handles.find(enqueue_node);
  if (it == resource_handles.end()) {
    // The corresponding queue was not found, there isn't much we can do.
    return Status::OK();
  }
  const NodeDef* qnode = it->second;
  auto qctx = shape_refiner->GetContext(qnode);
  if (!qctx) {
    return Status::OK();
  }
  auto* queue_handle_data = qctx->output_handle_shapes_and_types(0);

  // TODO(bsteiner): handle EnqueueMany as well.
  std::vector<ShapeAndType> shapes_and_types;
  for (int i = 1, end = ctx->input_types.size(); i < end; ++i) {
    GraphView::InputPort inp(enqueue_node, i);
    GraphView::OutputPort fanin = shape_refiner->graph().GetRegularFanin(inp);
    InferenceContext* in = shape_refiner->GetContext(fanin.node);
    ShapeHandle input = in->output(fanin.port_id);
    ctx->inference_context->SetInput(i, input);
    shapes_and_types.push_back({input, ctx->input_types[i]});
  }

  if (queue_handle_data == nullptr) {
    qctx->set_output_handle_shapes_and_types(0, shapes_and_types);
    *new_shapes = true;
  } else {
    TF_RETURN_IF_ERROR(RelaxEnqueueShapesAndMergeTypes(
        shape_refiner, qnode, *queue_handle_data, &shapes_and_types));
    *new_shapes |= !shape_refiner->EquivalentShapesAndTypes(*queue_handle_data,
                                                            shapes_and_types);
    qctx->set_output_handle_shapes_and_types(0, shapes_and_types);
  }

  return Status::OK();
}