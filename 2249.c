  TensorProto MakeIntegerScalarTensorProto(const DataType dtype,
                                           const int64_t val) {
    TensorProto tensor_proto;
    tensor_proto.set_dtype(dtype);
    // Scalar TensorProto has an empty tensor_shape; no dim, no dim.size.
    tensor_proto.mutable_tensor_shape();
    if (dtype == DT_INT32) {
      tensor_proto.add_int_val(val);
    } else if (dtype == DT_INT64) {
      tensor_proto.add_int64_val(val);
    }
    return tensor_proto;
  }