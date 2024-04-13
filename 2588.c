Status CheckFormatConstraintsOnShape(const TensorFormat tensor_format,
                                     const ShapeHandle shape_handle,
                                     const string& tensor_name,
                                     shape_inference::InferenceContext* c) {
  if (tensor_format == FORMAT_NCHW_VECT_C) {
    // Check that the vect dim has size 4 or 32.
    const int num_dims = c->Rank(shape_handle);
    DimensionHandle vect_dim = c->Dim(
        shape_handle, GetTensorInnerFeatureDimIndex(num_dims, tensor_format));
    int64_t vect_dim_val = c->Value(vect_dim);
    if (vect_dim_val != 4 && vect_dim_val != 32) {
      return errors::InvalidArgument(
          "VECT_C dimension must be 4 or 32, but is ", vect_dim_val);
    }
  }

  return Status::OK();
}