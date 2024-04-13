  Status UpdateFunction(const NodeDef* function_node) {
    NameAttrList function;
    TF_RETURN_IF_ERROR(NameAndAttrsFromFunctionCall(*function_node, &function));
    auto it = fun_to_grappler_function_item_.find(function.name());
    if (it == fun_to_grappler_function_item_.end()) {
      return errors::InvalidArgument(
          function.name(),
          " was not previously added to SymbolicShapeRefiner.");
    }

    const absl::optional<GrapplerFunctionItem>& maybe_grappler_function_item =
        it->second;
    if (!maybe_grappler_function_item.has_value()) {
      VLOG(3) << "Skip failed to instantiate function call: function_name="
              << function.name();

      auto* ctx = GetNodeContext(function_node);
      auto* ic = ctx->inference_context.get();
      for (int i = 0; i < ic->num_outputs(); ++i) {
        TF_RETURN_IF_ERROR(SetUnknownShape(function_node, i));
      }

      return Status::OK();
    }

    // Copy (not reference) so that changes we make here (e.g., replacing
    // _Arg with Const and _Retval with Identity) don't affect one in
    // fun_to_grappler_function_item_.
    GrapplerFunctionItem grappler_function_item = *maybe_grappler_function_item;
    MutableGraphView gv(&grappler_function_item.graph);

    // Forward shapes from function input nodes to argument nodes.
    for (int i = 0, end = grappler_function_item.inputs().size(); i < end;
         ++i) {
      auto& fun_input = grappler_function_item.input(i);
      NodeDef* fun_node = gv.GetNode(fun_input.node_name);
      const TensorId input_tensor = ParseTensorName(function_node->input(i));

      if (IsControlInput(input_tensor)) {
        return errors::FailedPrecondition(
            "Function inputs should not contain control nodes.");
      }

      const NodeDef* input_node = graph_.GetNode(input_tensor.node());
      if (input_node == nullptr) {
        return errors::FailedPrecondition(input_tensor.node(),
                                          " was not found in the graph.");
      }

      InferenceContext* input_ic = GetContext(input_node);
      if (input_ic == nullptr) {
        return errors::FailedPrecondition(
            "Inference context has not been created for ", input_tensor.node());
      }

      int output_port_num = input_tensor.index();
      AttrValue attr_output_shape;
      TensorShapeProto proto;
      const auto handle = input_ic->output(output_port_num);
      input_ic->ShapeHandleToProto(handle, &proto);
      // There may be dim.size < -1 in SymbolicShapeRefiner. Change those to -1.
      NormalizeShapeForOutput(&proto);
      // _Arg op's output shape uses _output_shapes attr.
      AttrValue output_attr;
      output_attr.mutable_list()->add_shape()->Swap(&proto);
      (*fun_node->mutable_attr())["_output_shapes"] = output_attr;

      // If dtype is DT_RESOURCE, ops that read _Arg op use _handle_dtypes and
      // _handle_shapes attr for its shapes and dtypes.
      if (fun_input.data_type == DT_RESOURCE) {
        auto* shapes_and_types =
            input_ic->output_handle_shapes_and_types(output_port_num);
        if (shapes_and_types != nullptr && !shapes_and_types->empty()) {
          AttrValue dtype_attr;
          AttrValue shape_attr;
          for (const auto& shape_and_type : *shapes_and_types) {
            const auto& dtype = shape_and_type.dtype;
            const auto& shape_handle = shape_and_type.shape;
            dtype_attr.mutable_list()->add_type(dtype);
            input_ic->ShapeHandleToProto(
                shape_handle, shape_attr.mutable_list()->add_shape());
          }
          (*fun_node->mutable_attr())["_handle_dtypes"] = dtype_attr;
          (*fun_node->mutable_attr())["_handle_shapes"] = shape_attr;
        } else {
          // Note that we do not return error here, even if the input node does
          // not have shapes_and_types. Within the function, we cannot infer the
          // output shape of the DT_RESOURCE input; hence, potentially unknown
          // shapes/dims in the function output shapes.
          VLOG(2)
              << "A function node (" << function_node->name()
              << ") has input with DT_RESOURCE, but the input node does not "
              << "have shapes_and_types information: \n"
              << "function_node: " << function_node->ShortDebugString() << "\n"
              << "function input: " << i
              << ", input node's output: " << output_port_num << "\n"
              << "input node: " << input_node->ShortDebugString();
        }
      }
    }

    // ReplaceInputWithConst() may break GraphView's internal node mapping
    // structure; hence, we separately build node name to NodeDef* map, for the
    // output nodes (before GraphView becomes invalid). Note that we use string,
    // not string_view.
    absl::flat_hash_map<std::string, NodeDef*> output_nodes;
    for (const auto& output_arg : grappler_function_item.outputs()) {
      output_nodes[output_arg.node_name] = gv.GetNode(output_arg.node_name);
    }

    // Replace input nodes with Consts, if values are known. Note that
    // we don't check exceptions here as it's done in the above loop.
    auto* ctx = GetNodeContext(function_node);
    auto* ic = ctx->inference_context.get();
    for (int i = grappler_function_item.inputs().size() - 1; i >= 0; --i) {
      const string& input = function_node->input(i);
      const string node_name = NodeName(input);
      const NodeDef* input_node = graph_.GetNode(node_name);
      if (IsConstant(*input_node)) {
        TF_CHECK_OK(
            ReplaceInputWithConst(*input_node, i, &grappler_function_item));
      } else if (static_cast<int>(ctx->input_tensor_protos.size()) > i &&
                 ctx->input_tensor_protos[i] != nullptr) {
        NodeDef const_input_node = MakeConstNodeDefFromTensorProto(
            ic, *ctx->input_tensor_protos[i], ctx->input_types[i]);
        TF_CHECK_OK(ReplaceInputWithConst(const_input_node, i,
                                          &grappler_function_item));
      } else if (static_cast<int>(ic->input_tensors_as_shapes().size()) > i &&
                 IsShapeFullyDefinedIntegerVectorOrScalar(
                     ic, ic->input(i), ic->input_tensors_as_shapes()[i],
                     ctx->input_types[i])) {
        // We have fully defined input_tensors_as_shapes for this input; use it
        // as a const input to the function node.
        NodeDef const_input_node = MakeConstNodeDefFromShape(
            ic, ic->input(i), ic->input_tensors_as_shapes()[i],
            ctx->input_types[i]);
        TF_CHECK_OK(ReplaceInputWithConst(const_input_node, i,
                                          &grappler_function_item));
      }
    }
    // node_name to NodeDef* map in GraphView gv can be broken due to
    // ReplaceInputWithConst(). gv should not be used after this.

    // Replace output _Retval nodes with Identity nodes. _Retval is a system op
    // without outputs and registered shape function.
    for (const auto& output_arg : grappler_function_item.outputs()) {
      NodeDef* output_node = output_nodes[output_arg.node_name];
      DCHECK_EQ(output_node->op(), "_Retval");
      output_node->set_op("Identity");
      output_node->mutable_attr()->erase("index");
    }

    // Perform inference on function body.
    GraphProperties gp(grappler_function_item);
    TF_RETURN_IF_ERROR(gp.InferStatically(
        /*assume_valid_feeds=*/true,
        /*aggressive_shape_inference=*/aggressive_shape_inference_,
        /*include_tensor_values=*/true));

    // Add return nodes for output shapes.
    int output = 0;
    ctx->output_tensors_as_shapes.resize(grappler_function_item.output_size());
    ctx->output_tensor_protos.resize(grappler_function_item.output_size(),
                                     nullptr);
    for (auto const& out_arg : grappler_function_item.outputs()) {
      // It is guaranteed that output_tensors does not contain any control
      // inputs, so port_id >= 0.
      TensorId out_tensor = ParseTensorName(out_arg.node_name);

      if (output_nodes.count(out_tensor.node()) <= 0) {
        return errors::FailedPrecondition(
            "Unable to find return function_node ", out_tensor.node(), " for ",
            function_node->name());
      }
      const NodeDef* retnode = output_nodes[out_tensor.node()];

      auto output_properties = gp.GetOutputProperties(retnode->name());
      int output_properties_size = output_properties.size();
      if (out_tensor.index() >= output_properties_size) {
        return errors::InvalidArgument(
            out_tensor.ToString(), " has invalid position ", out_tensor.index(),
            " (output_properties.size() = ", output_properties.size(), ").");
      }
      auto& outprop = output_properties[out_tensor.index()];
      TensorShapeProto shape = outprop.shape();
      NormalizeShapeForOutput(&shape);
      ShapeHandle out;
      TF_RETURN_IF_ERROR(ic->MakeShapeFromShapeProto(shape, &out));
      ic->set_output(output, out);
      if (outprop.has_value()) {
        // Forward tensor value to output_tensors_as_shape.
        MaybeTensorProtoToShape(ic, outprop.value(),
                                &ctx->output_tensors_as_shapes[output]);
        const_tensors_to_propagate_.push_back(outprop.value());
        ctx->output_tensor_protos[output] = &const_tensors_to_propagate_.back();
      }
      output++;
    }

    return Status::OK();
  }