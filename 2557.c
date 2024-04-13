Status FusedBatchNormShape(shape_inference::InferenceContext* c) {
  string data_format_str;
  TF_RETURN_IF_ERROR(c->GetAttr("data_format", &data_format_str));
  TensorFormat data_format;
  if (!FormatFromString(data_format_str, &data_format)) {
    return errors::InvalidArgument("Invalid data format string: ",
                                   data_format_str);
  }
  const int rank =
      (data_format_str == "NDHWC" || data_format_str == "NCDHW") ? 5 : 4;
  ShapeHandle x;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), rank, &x));

  bool is_training;
  TF_RETURN_IF_ERROR(c->GetAttr("is_training", &is_training));
  float exponential_avg_factor;
  if (!c->GetAttr("exponential_avg_factor", &exponential_avg_factor).ok()) {
    exponential_avg_factor = 1.0f;  // default value
  }
  int number_inputs = (is_training && exponential_avg_factor == 1.0f) ? 3 : 5;

  int channel_dim_index = GetTensorFeatureDimIndex(rank, data_format);
  DimensionHandle channel_dim = c->Dim(x, channel_dim_index);

  // covers scale, offset, and if is_training is false, mean, variance
  for (int i = 1; i < number_inputs; ++i) {
    ShapeHandle vec;
    TF_RETURN_IF_ERROR(c->WithRank(c->input(i), 1, &vec));
    TF_RETURN_IF_ERROR(c->Merge(channel_dim, c->Dim(vec, 0), &channel_dim));
  }

  ShapeHandle y;
  TF_RETURN_IF_ERROR(c->ReplaceDim(x, channel_dim_index, channel_dim, &y));
  c->set_output(0, y);
  ShapeHandle vector_shape = c->Vector(channel_dim);
  c->set_output(1, vector_shape);
  c->set_output(2, vector_shape);
  c->set_output(3, vector_shape);
  c->set_output(4, vector_shape);
  return Status::OK();
}