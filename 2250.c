Status GraphProperties::UpdateQueue(const NodeDef* queue_node,
                                    SymbolicShapeRefiner* shape_refiner,
                                    bool* new_shapes) {
  auto* ctx = shape_refiner->GetNodeContext(queue_node);
  if (!ctx) {
    TF_RETURN_IF_ERROR(shape_refiner->AddNode(queue_node));
    ctx = CHECK_NOTNULL(shape_refiner->GetNodeContext(queue_node));
  }
  auto* ic = ctx->inference_context.get();

  auto* outputs = ic->output_handle_shapes_and_types(0);
  if (outputs) {
    // Shapes and types are already set, presumably by Enqueue ops.
    return shape_refiner->UpdateNode(queue_node, new_shapes);
  }

  if (queue_node->attr().count("shapes") <= 0 ||
      queue_node->attr().count("component_types") <= 0 ||
      queue_node->attr().at("shapes").list().shape_size() !=
          queue_node->attr().at("component_types").list().type_size()) {
    // Errors in shapes and component_types attr.
    return shape_refiner->UpdateNode(queue_node, new_shapes);
  }

  // Extract types and shapes from Queue attr.
  const auto& shapes = queue_node->attr().at("shapes").list().shape();
  const auto& types = queue_node->attr().at("component_types").list().type();
  std::vector<ShapeAndType> shapes_and_types;
  for (int i = 0; i < types.size(); i++) {
    const auto& shape = shapes[i];
    ShapeHandle shape_handle;
    TF_RETURN_IF_ERROR(
        ic->MakeShapeFromPartialTensorShape(shape, &shape_handle));
    DataType data_type =
        queue_node->attr().at("component_types").list().type(i);
    ShapeAndType shape_and_type(shape_handle, data_type);
    shapes_and_types.push_back(shape_and_type);
  }
  ic->set_output_handle_shapes_and_types(0, shapes_and_types);

  // Queue node is updated with output_handle_shapes_and_types, so set
  // new_shapes and ignore it from UpdateNoe().
  *new_shapes = true;
  bool dummy_new_shapes = false;
  return shape_refiner->UpdateNode(queue_node, &dummy_new_shapes);
}