Status FusedBatchNormGradExShape(shape_inference::InferenceContext* c) {
  TF_RETURN_IF_ERROR(FusedBatchNormGradShape(c));

  int num_side_inputs;
  TF_RETURN_IF_ERROR(c->GetAttr("num_side_inputs", &num_side_inputs));
  if (num_side_inputs == 0) {
    return Status::OK();
  }

  string data_format_str;
  TF_RETURN_IF_ERROR(c->GetAttr("data_format", &data_format_str));
  TensorFormat data_format;
  if (!FormatFromString(data_format_str, &data_format)) {
    return errors::InvalidArgument("Invalid data format string: ",
                                   data_format_str);
  }
  const int rank =
      (data_format_str == "NDHWC" || data_format_str == "NCDHW") ? 5 : 4;
  ShapeHandle y_backprop;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), rank, &y_backprop));
  ShapeHandle x;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), rank, &x));

  int channel_dim_index = GetTensorFeatureDimIndex(rank, data_format);
  DimensionHandle channel_dim = c->Dim(y_backprop, channel_dim_index);
  TF_RETURN_IF_ERROR(
      c->Merge(channel_dim, c->Dim(x, channel_dim_index), &channel_dim));

  ShapeHandle side_input_backprop;
  TF_RETURN_IF_ERROR(c->ReplaceDim(y_backprop, channel_dim_index, channel_dim,
                                   &side_input_backprop));

  c->set_output(5, side_input_backprop);
  return Status::OK();
}