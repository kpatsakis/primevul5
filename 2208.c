  bool MaybeTensorProtoToShape(InferenceContext* ic,
                               const TensorProto& tensor_proto,
                               ShapeHandle* tensors_as_shapes) {
    // Skip if dtype is not integer.
    if (tensor_proto.dtype() != DT_INT32 && tensor_proto.dtype() != DT_INT64) {
      return false;
    }
    // Skip if the const tensor is too large.
    if (NumElementsFromTensorProto(tensor_proto) >
        kThresholdToSkipConstTensorInstantiation) {
      return false;
    }
    // Skip if shape is neither scalar nor vector.
    if (tensor_proto.tensor_shape().unknown_rank() ||
        tensor_proto.tensor_shape().dim_size() > 1) {
      return false;
    }
    Tensor tensor;
    if (!tensor.FromProto(tensor_proto)) {
      return false;
    }
    return MaybeTensorValueToShape(ic, tensor, tensors_as_shapes);
  }