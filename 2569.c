Status MakeShapeFromFormat(TensorFormat format, DimensionOrConstant N,
                           const std::vector<DimensionOrConstant>& spatial,
                           DimensionOrConstant C, ShapeHandle* out,
                           shape_inference::InferenceContext* context) {
  const int num_dims = GetTensorDimsFromSpatialDims(spatial.size(), format);
  std::vector<DimensionHandle> dims_actual(num_dims);
  dims_actual[GetTensorBatchDimIndex(num_dims, format)] = context->MakeDim(N);
  int outer_c_index = GetTensorFeatureDimIndex(num_dims, format);
  dims_actual[outer_c_index] = context->MakeDim(C);
  if (format == FORMAT_NCHW_VECT_C) {
    dims_actual[GetTensorInnerFeatureDimIndex(num_dims, format)] =
        context->MakeDim(4);
  } else if (format == FORMAT_NHWC_VECT_W) {
    dims_actual[GetTensorInnerWidthDimIndex(num_dims, format)] =
        context->MakeDim(4);
  }
  for (int spatial_dim = 0, end = spatial.size(); spatial_dim < end;
       spatial_dim++) {
    dims_actual[GetTensorSpatialDimIndex(num_dims, format, spatial_dim)] =
        context->MakeDim(spatial[spatial_dim]);
  }
  *out = context->MakeShape(dims_actual);
  return Status::OK();
}