  Status UpdateOutputShapesAndValues(const NodeDef& node, NodeContext* c) {
    InferenceContext* ic = c->inference_context.get();

    // Input to EvaluateNode()
    TensorVector inputs;
    // Container for temporarily created tensor object.
    std::vector<Tensor> input_tensor_vector(ic->num_inputs());
    CreateInputTensors(c, &input_tensor_vector, &inputs);

    // Output for EvaluateNode() and output tensor clean up object.
    TensorVector outputs;
    auto outputs_cleanup = gtl::MakeCleanup([&outputs] {
      for (const auto& output : outputs) {
        if (output.tensor) {
          delete output.tensor;
        }
      }
    });

    TF_RETURN_IF_ERROR(EvaluateNode(node, inputs, /*cpu_device=*/nullptr,
                                    &resource_mgr_, &outputs));
    c->output_tensors_as_shapes.resize(outputs.size());
    c->output_tensor_protos.resize(outputs.size(), nullptr);
    for (int k = 0, outputs_size = outputs.size(); k < outputs_size; k++) {
      const auto& t = outputs[k];
      // Override output shape.
      ShapeHandle output_shape;
      TF_RETURN_IF_ERROR(
          ic->MakeShapeFromTensorShape(t->shape(), &output_shape));
      if (ic->FullyDefined(ic->output(k)) &&
          !EquivalentShapes(ic->output(k), output_shape)) {
        LOG(WARNING) << "UpdateOutputShapesAndValues() -- node: " << node.name()
                     << ", inferred output shape "
                     << "doesn't match for k=" << k << ": "
                     << "ic->output(k): " << ic->DebugString(ic->output(k))
                     << ", output_shape: " << ic->DebugString(output_shape)
                     << " -- " << node.DebugString();
      }
      ic->set_output(k, output_shape);
      // Set output_tensors_as_shape.
      MaybeTensorValueToShape(ic, *t.tensor, &c->output_tensors_as_shapes[k]);

      // Set output_tensor_protos.
      TensorProto tensor_proto;
      t->AsProtoTensorContent(&tensor_proto);
      const_tensors_to_propagate_.push_back(tensor_proto);
      c->output_tensor_protos[k] = &const_tensors_to_propagate_.back();
    }
    return Status::OK();
  }