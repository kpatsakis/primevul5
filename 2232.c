TensorProto MakeTensorProtoFromShape(InferenceContext* ic,
                                     const ShapeHandle& shape,
                                     const ShapeHandle& tensor_as_shape,
                                     const DataType& dtype) {
  TensorProto tensor_proto;
  tensor_proto.set_dtype(dtype);
  auto* shape_proto = tensor_proto.mutable_tensor_shape();
  if (ic->Rank(shape) == 1) {
    shape_proto->add_dim()->set_size(ic->Rank(tensor_as_shape));
  }
  // For a scalar tensor, tensor_shape field will be left empty; no dim.
  for (int i = 0; i < ic->Rank(tensor_as_shape); i++) {
    int64_t value = ic->Value(ic->Dim(tensor_as_shape, i));
    if (dtype == DT_INT32) {
      tensor_proto.add_int_val(value);
    } else {
      tensor_proto.add_int64_val(value);
    }
  }
  return tensor_proto;
}