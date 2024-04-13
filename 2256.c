    std::string DebugString(const NodeDef& node) {
      std::string output;
      auto* ic = inference_context.get();
      absl::StrAppend(
          &output, node.name(), " [", node.op(), "] has ", ic->num_inputs(),
          (ic->num_inputs() > 1 ? " inputs and " : " input and "),
          ic->num_outputs(), (ic->num_outputs() > 1 ? " outputs" : " output"));
      if (op_data->is_function_op) {
        absl::StrAppend(&output, " (function op)");
      }
      absl::StrAppend(&output, ": \n");

      for (int i = 0; i < ic->num_inputs(); i++) {
        absl::StrAppend(&output, " input [", i, "] ", node.input(i),
                        " -- type: ", DataTypeString(input_types.at(i)),
                        ", shape: ", ic->DebugString(ic->input(i)),
                        ", tensor: ");
        Tensor t1;
        int input_tensor_protos_size = input_tensor_protos.size();
        if (input_tensor_protos_size > i &&
            input_tensor_protos.at(i) != nullptr &&
            t1.FromProto(*input_tensor_protos.at(i))) {
          absl::StrAppend(&output, t1.DebugString(), ", tensor_as_shape: ");
        } else {
          absl::StrAppend(&output, " null, tensor_as_shape: ");
        }
        int input_tensors_as_shapes_to_propagate_size =
            input_tensors_as_shapes_to_propagate.size();
        if (input_tensors_as_shapes_to_propagate_size > i) {
          absl::StrAppend(
              &output,
              StringifyShapeHandle(input_tensors_as_shapes_to_propagate.at(i)),
              "\n");
        } else {
          absl::StrAppend(&output, " null\n");
        }
      }
      for (int i = 0; i < ic->num_outputs(); i++) {
        absl::StrAppend(&output, " output [", i,
                        "] -- type: ", DataTypeString(output_types.at(i)),
                        ", shape: ", ic->DebugString(ic->output(i)),
                        ", tensor: ");
        Tensor t2;
        int output_tensor_protos_size = output_tensor_protos.size();
        if (output_tensor_protos_size > i &&
            output_tensor_protos.at(i) != nullptr &&
            t2.FromProto(*output_tensor_protos.at(i))) {
          absl::StrAppend(&output, t2.DebugString(), ", tensor_as_shape: ");
        } else {
          absl::StrAppend(&output, " null, tensor_as_shape: ");
        }
        int output_tensors_as_shapes_size = output_tensors_as_shapes.size();
        if (output_tensors_as_shapes_size > i) {
          absl::StrAppend(&output,
                          StringifyShapeHandle(output_tensors_as_shapes.at(i)),
                          "\n");
        } else {
          absl::StrAppend(&output, " null\n");
        }
      }
      return output;
    }