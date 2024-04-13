Status FusedBatchNormGradShape(shape_inference::InferenceContext* c) {
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

  bool is_training;
  TF_RETURN_IF_ERROR(c->GetAttr("is_training", &is_training));

  int channel_dim_index = GetTensorFeatureDimIndex(rank, data_format);
  DimensionHandle channel_dim = c->Dim(y_backprop, channel_dim_index);
  TF_RETURN_IF_ERROR(
      c->Merge(channel_dim, c->Dim(x, channel_dim_index), &channel_dim));

  // covers scale, mean (reserve_space_1), variance (reserve_space_2)
  for (int i = 2; i < 5; ++i) {
    ShapeHandle vec;
    TF_RETURN_IF_ERROR(c->WithRank(c->input(i), 1, &vec));
    TF_RETURN_IF_ERROR(c->Merge(channel_dim, c->Dim(vec, 0), &channel_dim));
  }

  ShapeHandle x_backprop;
  TF_RETURN_IF_ERROR(
      c->ReplaceDim(y_backprop, channel_dim_index, channel_dim, &x_backprop));
  c->set_output(0, x_backprop);
  c->set_output(1, c->Vector(channel_dim));
  c->set_output(2, c->Vector(channel_dim));
  c->set_output(3, c->Vector(0));
  c->set_output(4, c->Vector(0));
  return Status::OK();
}