  Status AddNode(const NodeDef* node) {
    NodeContext& node_ctx = node_to_context_[node];
    NameAttrList function;
    TF_RETURN_IF_ERROR(NameAndAttrsFromFunctionCall(*node, &function));

    // For PartitionedCall, op_data represents the function info.
    TF_RETURN_IF_ERROR(
        function_library_.LookUp(function.name(), &node_ctx.op_data));

    if (node_ctx.op_data->is_function_op) {
      TF_RETURN_IF_ERROR(AddFunction(node, function));
    }

    TF_RETURN_IF_ERROR(InOutTypesForNode(*node, node_ctx.op_data->op_def,
                                         &node_ctx.input_types,
                                         &node_ctx.output_types));

    // Create the inference context for this node.
    const int num_inputs = node_ctx.input_types.size();
    std::vector<ShapeHandle> input_shapes(num_inputs);
    std::vector<std::unique_ptr<std::vector<ShapeAndType>>>
        input_handle_shapes_and_types(num_inputs);
    std::vector<const Tensor*> input_tensors(num_inputs, nullptr);
    std::vector<ShapeHandle> input_tensors_as_shapes;

    node_ctx.inference_context.reset(new InferenceContext(
        graph_def_version_, *node, node_ctx.op_data->op_def, input_shapes,
        input_tensors, input_tensors_as_shapes,
        std::move(input_handle_shapes_and_types)));
    const Status s = node_ctx.inference_context->construction_status();
    if (!s.ok()) {
      node_ctx.inference_context.reset(nullptr);
    }
    return s;
  }