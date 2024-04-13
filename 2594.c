Status Conv2DBackpropInputShape(shape_inference::InferenceContext* c) {
  string data_format_str;
  if (!c->GetAttr("data_format", &data_format_str).ok()) {
    data_format_str = "NHWC";
  }
  TensorFormat data_format;
  if (!FormatFromString(data_format_str, &data_format)) {
    return errors::InvalidArgument("Invalid data format string: ",
                                   data_format_str);
  }

  // For the rest of this function, output_grad_* describes out_backprop and
  // input_grad_* describes in_backprop.
  ShapeHandle output_grad_shape = c->input(2);
  TF_RETURN_IF_ERROR(c->WithRank(output_grad_shape, 4, &output_grad_shape));
  ShapeHandle filter_shape = c->input(1);
  TF_RETURN_IF_ERROR(c->WithRank(filter_shape, 4, &filter_shape));

  DimensionHandle batch_size_dim;
  DimensionHandle output_grad_depth_dim;
  gtl::InlinedVector<DimensionHandle, 2> output_grad_spatial_dims(2);
  TF_RETURN_IF_ERROR(DimensionsFromShape(
      output_grad_shape, data_format, &batch_size_dim,
      absl::MakeSpan(output_grad_spatial_dims), &output_grad_depth_dim, c));
  DimensionHandle unused;
  TF_RETURN_IF_ERROR(
      c->Merge(output_grad_depth_dim, c->Dim(filter_shape, 3), &unused));

  ShapeHandle specified_input_grad_shape;
  TF_RETURN_IF_ERROR(
      c->MakeShapeFromShapeTensor(0, &specified_input_grad_shape));
  if (c->Rank(specified_input_grad_shape) == InferenceContext::kUnknownRank) {
    TF_RETURN_IF_ERROR(c->WithRank(specified_input_grad_shape, 4,
                                   &specified_input_grad_shape));
  }

  // input_grad_depth_dim doesn't equal c->Dim(filter_shape,2) when the number
  // of groups is larger than 1. If input_sizes is a 4D shape, we collect
  // input_grad_depth_dim from input_sizes; otherwise we compute it as
  // c->Dim(filter_shape,2).
  DimensionHandle input_grad_depth_dim;
  gtl::InlinedVector<DimensionHandle, 2> specified_input_grad_spatial_dims(2);
  int specified_input_grad_rank = c->Rank(specified_input_grad_shape);
  if (specified_input_grad_rank == 4) {
    DimensionHandle specified_batch_size_dim;
    TF_RETURN_IF_ERROR(DimensionsFromShape(
        specified_input_grad_shape, data_format, &specified_batch_size_dim,
        absl::MakeSpan(specified_input_grad_spatial_dims),
        &input_grad_depth_dim, c));
    TF_RETURN_IF_ERROR(
        c->Merge(specified_batch_size_dim, batch_size_dim, &unused));
  } else if (specified_input_grad_rank == 2) {
    specified_input_grad_spatial_dims[0] =
        c->Dim(specified_input_grad_shape, 0);
    specified_input_grad_spatial_dims[1] =
        c->Dim(specified_input_grad_shape, 1);
    input_grad_depth_dim = c->Dim(filter_shape, 2);
  } else {
    return errors::InvalidArgument(
        "Conv2DBackpropInput requires input_sizes to contain 4 values or 2 "
        "values, but got: ",
        specified_input_grad_rank);
  }

  ShapeHandle input_grad_shape;
  TF_RETURN_IF_ERROR(ShapeFromDimensions(
      batch_size_dim, specified_input_grad_spatial_dims, input_grad_depth_dim,
      data_format, /*vect_size=*/absl::nullopt, c, &input_grad_shape));
  c->set_output(0, input_grad_shape);
  return Status::OK();
}