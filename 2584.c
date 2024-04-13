Status DimensionsFromShape(ShapeHandle shape, TensorFormat format,
                           DimensionHandle* batch_dim,
                           gtl::MutableArraySlice<DimensionHandle> spatial_dims,
                           DimensionHandle* filter_dim,
                           InferenceContext* context) {
  const int32_t rank =
      GetTensorDimsFromSpatialDims(spatial_dims.size(), format);
  // Batch.
  *batch_dim = context->Dim(shape, GetTensorBatchDimIndex(rank, format));
  // Spatial.
  for (int spatial_dim_index = 0, end = spatial_dims.size();
       spatial_dim_index < end; ++spatial_dim_index) {
    spatial_dims[spatial_dim_index] = context->Dim(
        shape, GetTensorSpatialDimIndex(rank, format, spatial_dim_index));
  }
  // Channel.
  *filter_dim = context->Dim(shape, GetTensorFeatureDimIndex(rank, format));
  if (format == FORMAT_NCHW_VECT_C) {
    TF_RETURN_IF_ERROR(context->Multiply(
        *filter_dim,
        context->Dim(shape, GetTensorInnerFeatureDimIndex(rank, format)),
        filter_dim));
  }
  return Status::OK();
}