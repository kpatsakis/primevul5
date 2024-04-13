Status FusedBatchNormExShape(shape_inference::InferenceContext* c) {
  TF_RETURN_IF_ERROR(FusedBatchNormV3Shape(c));

  string data_format_str;
  TF_RETURN_IF_ERROR(c->GetAttr("data_format", &data_format_str));
  TensorFormat data_format;
  if (!FormatFromString(data_format_str, &data_format)) {
    return errors::InvalidArgument("Invalid data format string: ",
                                   data_format_str);
  }
  ShapeHandle x;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 4, &x));

  int channel_dim_index = GetTensorFeatureDimIndex(4, data_format);
  DimensionHandle channel_dim = c->Dim(x, channel_dim_index);

  // This is a cuDNN implementation constraint.
  if (c->ValueKnown(channel_dim) && c->Value(channel_dim) % 4 != 0) {
    return errors::InvalidArgument(
        "_FusedBatchNormEx channel dimension must be divisible by 4.");
  }

  return Status::OK();
}