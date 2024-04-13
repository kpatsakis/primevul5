  Status UpdateOutputShapesUsingAnnotatedInformation(const NodeDef& node,
                                                     NodeContext* c) const {
    const auto& attr = node.attr();
    if (attr.count(kOutputSame) == 0 || !attr.at(kOutputSame).b() ||
        attr.count(kOutputShapes) == 0)
      return Status::OK();

    InferenceContext* ic = c->inference_context.get();
    int output_size = attr.at(kOutputShapes).list().shape_size();

    for (int i = 0; i < ic->num_outputs(); i++) {
      // Annotated Switch node has only one output. Propagate the shape to all
      // the outputs.
      int shape_index = IsSwitch(node) ? 0 : i;
      if (shape_index >= output_size) {
        LOG(WARNING)
            << "UpdateOutputShapesUsingAnnotatedInformation() -- node: "
            << node.name() << ", inferred output shape size "
            << ic->num_outputs() << ", annotated output shape size "
            << output_size;
        break;
      }

      const TensorShapeProto& shape =
          attr.at(kOutputShapes).list().shape(shape_index);
      if (shape.dim().empty()) continue;

      ShapeHandle output_shape;
      TF_RETURN_IF_ERROR(ic->MakeShapeFromShapeProto(shape, &output_shape));

      // Check if annotated shapes are incompatible with inferred shapes.
      if ((ic->FullyDefined(ic->output(i)) &&
           !SameShapes(ic->output(i), output_shape)) ||
          (!ic->FullyDefined(ic->output(i)) &&
           !CompatibleShapes(ic->output(i), output_shape))) {
        LOG(WARNING)
            << "UpdateOutputShapesUsingAnnotatedInformation() -- node: "
            << node.name() << ", inferred output shape "
            << "doesn't match for i=" << i << ": "
            << "ic->output(k): " << ic->DebugString(ic->output(i))
            << ", annotated output shape: " << ic->DebugString(output_shape)
            << " -- " << node.DebugString();
        c->shape_incompatible = true;
      }

      // Only use annotated shapes if the inference shape is unknown and
      // compatible with annotated shapes.
      if (!ic->FullyDefined(ic->output(i)) &&
          CompatibleShapes(ic->output(i), output_shape)) {
        VLOG(3) << "UpdateOutputShapesUsingAnnotatedInformation() -- node: "
                << node.name() << ", inferred output shape " << i << ": "
                << "ic->output(i): " << ic->DebugString(ic->output(i))
                << ", annotated output shape: " << ic->DebugString(output_shape)
                << " -- " << node.ShortDebugString();
        ic->set_output(i, output_shape);
      }
    }

    return Status::OK();
  }