  Status MaybeUpdateNodeContextOutput(const NodeDef& node, const bool is_fed,
                                      NodeContext* c) {
    // Propagate tensors and shape tensors unless the node is fed.
    // TODO(bsteiner) We should still propagate the shapes to the ports that
    // aren't fed in the case of a ShapeN node.

    // Note that when propagating tensors_as_shapes, we use
    // c->input_tensors_as_shapes_to_progate instead of
    // ic->input_tensors_as_shapes. The former uses kUnknownDimFromConst if
    // UnknownDim is from Const tensor, and it is propagated through shape
    // inference. Before calling shape functions, we convert it to UnknownDim,
    // but instantiate a new UnknownDim to prevent incorrect symbolic shape
    // inference through UnknownDim from Const.
    InferenceContext* ic = c->inference_context.get();
    if (!is_fed) {
      if (IsConstant(node)) {
        const TensorProto& tensor_proto = node.attr().at("value").tensor();
        c->output_tensor_protos.resize(1);
        c->output_tensor_protos[0] = &tensor_proto;
        c->output_tensors_as_shapes.resize(1);
        MaybeTensorProtoToShape(ic, tensor_proto,
                                &c->output_tensors_as_shapes[0]);
      } else if (IsRank(node)) {
        if (ic->RankKnown(ic->input(0))) {
          // Propagate rank value.
          int32_t rank = ic->Rank(ic->input(0));
          const_tensors_to_propagate_.push_back(
              MakeIntegerScalarTensorProto(DT_INT32, rank));
          c->output_tensor_protos.resize(1);
          c->output_tensor_protos[0] = &const_tensors_to_propagate_.back();
        }
      } else if (IsSize(node)) {
        DimensionHandle size = ic->NumElements(ic->input(0));
        if (ic->ValueKnown(size)) {
          // Propagate size value.
          int64_t sz = ic->Value(size);
          bool valid = false;
          if (node.attr().at("out_type").type() == DT_INT32) {
            if (sz < std::numeric_limits<int32>::max()) {
              const_tensors_to_propagate_.push_back(
                  MakeIntegerScalarTensorProto(DT_INT32, sz));
              valid = true;
            }
          } else {
            const_tensors_to_propagate_.push_back(
                MakeIntegerScalarTensorProto(DT_INT64, sz));
            valid = true;
          }
          if (valid) {
            c->output_tensor_protos.resize(1);
            c->output_tensor_protos[0] = &const_tensors_to_propagate_.back();
          }
        }
      } else if (IsShape(node)) {
        c->output_tensors_as_shapes.resize(1);
        c->output_tensors_as_shapes[0] = c->inference_context->input(0);
      } else if (IsShapeN(node)) {
        c->output_tensors_as_shapes.resize(c->inference_context->num_inputs());
        for (int i = 0; i < c->inference_context->num_inputs(); ++i) {
          c->output_tensors_as_shapes[i] = c->inference_context->input(i);
        }
      } else if (node.op() == "ConcatV2") {
        bool valid = true;
        ShapeHandle result;
        for (int i = 0; i < ic->num_inputs() - 1; ++i) {
          ShapeHandle input = c->input_tensors_as_shapes_to_propagate[i];
          if (!ic->RankKnown(input)) {
            valid = false;
            break;
          } else if (i == 0) {
            result = input;
          } else {
            TF_RETURN_IF_ERROR(ic->Concatenate(result, input, &result));
          }
        }
        if (valid) {
          c->output_tensors_as_shapes.resize(1);
          c->output_tensors_as_shapes[0] = result;
        }
      } else if (IsPack(node)) {
        // A Pack node concatenating scalars is often used to generate a shape.
        std::vector<DimensionHandle> dims;
        bool valid = true;
        for (int i = 0; i < ic->num_inputs(); ++i) {
          const Tensor* t = ic->input_tensor(i);
          if (t) {
            if (t->dims() != 0 ||
                (t->dtype() != DT_INT32 && t->dtype() != DT_INT64)) {
              valid = false;
              break;
            }
            int64_t size = t->dtype() == DT_INT32 ? t->scalar<int32>()()
                                                  : t->scalar<int64_t>()();
            dims.push_back(size < 0 ? ic->MakeDim(kUnknownDimFromConst)
                                    : ic->MakeDim(size));
          } else {
            // Don't have tensor value, but use input_tensors_as_shapes, if
            // possible.
            const ShapeHandle& shape_handle =
                c->input_tensors_as_shapes_to_propagate[i];
            if (ic->RankKnown(shape_handle) && ic->Rank(shape_handle) >= 1 &&
                ic->ValueKnown(ic->Dim(shape_handle, 0))) {
              dims.push_back(ic->Dim(shape_handle, 0));
            } else {
              // This is not from Const, but as it shouldn'be used as symbolic
              // unknown dim for different ops, we use kUnknownDimFromConst.
              dims.push_back(ic->MakeDim(kUnknownDimFromConst));
            }
          }
        }
        if (valid) {
          c->output_tensors_as_shapes.resize(1);
          c->output_tensors_as_shapes[0] = ic->MakeShape(dims);
        }
      } else if (IsIdentity(node) || IsIdentityNSingleInput(node)) {
        c->output_tensors_as_shapes.resize(1);
        c->output_tensors_as_shapes[0] =
            c->input_tensors_as_shapes_to_propagate[0];
        if (c->input_tensor_protos[0] != nullptr) {
          c->output_tensor_protos.resize(1);
          c->output_tensor_protos[0] = c->input_tensor_protos[0];
        }
      } else if (IsSlice(node)) {
        ShapeHandle input = c->input_tensors_as_shapes_to_propagate[0];
        bool valid = ic->RankKnown(input);
        const Tensor* slice_offset = ic->input_tensor(1);
        valid &= slice_offset != nullptr && slice_offset->NumElements() == 1;
        const Tensor* slice_size = ic->input_tensor(2);
        valid &= slice_size != nullptr && slice_size->NumElements() == 1;
        if (valid) {
          int64_t start = slice_offset->dtype() == DT_INT32
                              ? slice_offset->flat<int32>()(0)
                              : slice_offset->flat<int64_t>()(0);
          int64_t size = (slice_size->dtype() == DT_INT32
                              ? slice_size->flat<int32>()(0)
                              : slice_size->flat<int64_t>()(0));
          ShapeHandle result;
          if (size == -1) {
            TF_RETURN_IF_ERROR(ic->Subshape(input, start, &result));
          } else {
            int64_t end = start + size;
            TF_RETURN_IF_ERROR(ic->Subshape(input, start, end, &result));
          }
          c->output_tensors_as_shapes.resize(1);
          c->output_tensors_as_shapes[0] = result;
        }
      } else if (IsStridedSlice(node)) {
        ShapeHandle input = c->input_tensors_as_shapes_to_propagate[0];
        bool valid = ic->RankKnown(input);
        const Tensor* slice_begin = ic->input_tensor(1);
        valid &= slice_begin != nullptr && slice_begin->NumElements() == 1;
        const Tensor* slice_end = ic->input_tensor(2);
        valid &= slice_end != nullptr && slice_end->NumElements() == 1;
        const Tensor* slice_stride = ic->input_tensor(3);
        valid &= slice_stride != nullptr && slice_stride->NumElements() == 1;

        if (node.attr().count("ellipsis_mask") > 0 &&
            node.attr().at("ellipsis_mask").i() != 0) {
          valid = false;
        }
        if (node.attr().count("new_axis_mask") > 0 &&
            node.attr().at("new_axis_mask").i() != 0) {
          valid = false;
        }
        if (node.attr().count("shrink_axis_mask") > 0 &&
            node.attr().at("shrink_axis_mask").i() != 0) {
          valid = false;
        }
        int begin_mask = 0;
        if (node.attr().count("begin_mask") > 0) {
          begin_mask = node.attr().at("begin_mask").i();
        }
        int end_mask = 0;
        if (node.attr().count("end_mask") > 0) {
          end_mask = node.attr().at("end_mask").i();
        }
        if (begin_mask < 0 || begin_mask > 1 || end_mask < 0 || end_mask > 1) {
          valid = false;
        }
        if (valid) {
          int64_t begin = 0;
          if (begin_mask == 0) {
            begin = slice_begin->dtype() == DT_INT32
                        ? slice_begin->flat<int32>()(0)
                        : slice_begin->flat<int64_t>()(0);
          }
          int64_t end = std::numeric_limits<int64_t>::max();
          if (end_mask == 0) {
            end = (slice_end->dtype() == DT_INT32
                       ? slice_end->flat<int32>()(0)
                       : slice_end->flat<int64_t>()(0));
          }
          int64_t stride = slice_stride->dtype() == DT_INT32
                               ? slice_stride->flat<int32>()(0)
                               : slice_stride->flat<int64_t>()(0);
          ShapeHandle result;
          TF_RETURN_IF_ERROR(ic->Subshape(input, begin, end, stride, &result));
          c->output_tensors_as_shapes.resize(1);
          c->output_tensors_as_shapes[0] = result;
        }
      }
    }

    if (aggressive_shape_inference_) {
      // Update output shapes with annotated information. This is optional.
      UpdateOutputShapesUsingAnnotatedInformation(node, c).IgnoreError();

      // Update output tensor values using EvaluateNode() if we can.
      // Due to the cost of EvaluateNode(), we run it only for certain op types
      // (white listed) and small integer tensors.

      const int max_element_size = 17;  // Max up to 4x4 matrix or similar.
      if (AllOutputValuesKnown(c) || !AllInputValuesKnown(c) ||
          !ShouldUpdateOutputShapesAndValues(c, max_element_size)) {
        return Status::OK();
      }
      UpdateOutputShapesAndValues(node, c).IgnoreError();  // This is optional.
    }
    return Status::OK();
  }