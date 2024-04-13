  Status InferShapes(const NodeDef& node, NodeContext* c) {
    // Infer the shapes of output tensors.
    if (!c->op_data || c->op_data->shape_inference_fn == nullptr ||
        !c->inference_context->Run(c->op_data->shape_inference_fn).ok()) {
      // Annotate outputs with unknown shapes. Update output shapes with
      // annotated information later on if available.
      // Note that shape inference function may return an error, but we ignore
      // it, and use UnknownShape in that case.
      TF_RETURN_IF_ERROR(
          c->inference_context->Run(shape_inference::UnknownShape));
    }
    Status status = Status::OK();
    auto it = fed_ports_.find(node.name());
    const bool is_fed = it != fed_ports_.end();
    if (is_fed) {
      // It is possible to feed node output ports with tensors of any shape: as
      // a result, the shape of a fed port is completely unknown.
      for (const int output_port : it->second) {
        status.Update(SetUnknownShape(&node, output_port));
      }
    }

    // Update NodeContext output fields after shape inference function runs.
    status.Update(MaybeUpdateNodeContextOutput(node, is_fed, c));

    return status;
  }