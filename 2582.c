Status MaxPoolShapeImpl(shape_inference::InferenceContext* c,
                        bool supports_explicit_padding) {
  string data_format_str;
  TensorFormat data_format;
  Status s = c->GetAttr("data_format", &data_format_str);
  if (s.ok()) {
    FormatFromString(data_format_str, &data_format);
  } else {
    data_format = FORMAT_NHWC;
  }

  const int rank = (data_format == FORMAT_NCHW_VECT_C) ? 5 : 4;
  ShapeHandle input_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), rank, &input_shape));

  TF_RETURN_IF_ERROR(
      CheckFormatConstraintsOnShape(data_format, input_shape, "input", c));

  std::vector<int32> strides;
  TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));
  if (strides.size() != 4) {
    return errors::InvalidArgument(
        "MaxPool requires the stride attribute to contain 4 values, but got: ",
        strides.size());
  }

  std::vector<int32> kernel_sizes;
  TF_RETURN_IF_ERROR(c->GetAttr("ksize", &kernel_sizes));
  if (kernel_sizes.size() != 4) {
    return errors::InvalidArgument(
        "MaxPool requires the ksize attribute to contain 4 values, but got: ",
        kernel_sizes.size());
  }

  int32_t stride_depth = GetTensorDim(strides, data_format, 'C');
  int32_t stride_rows = GetTensorDim(strides, data_format, 'H');
  int32_t stride_cols = GetTensorDim(strides, data_format, 'W');
  int32_t kernel_depth = GetTensorDim(kernel_sizes, data_format, 'C');
  int32_t kernel_rows = GetTensorDim(kernel_sizes, data_format, 'H');
  int32_t kernel_cols = GetTensorDim(kernel_sizes, data_format, 'W');

  constexpr int num_spatial_dims = 2;
  DimensionHandle batch_size_dim = c->Dim(
      input_shape, GetTensorDimIndex<num_spatial_dims>(data_format, 'N'));
  DimensionHandle in_rows_dim = c->Dim(
      input_shape, GetTensorDimIndex<num_spatial_dims>(data_format, 'H'));
  DimensionHandle in_cols_dim = c->Dim(
      input_shape, GetTensorDimIndex<num_spatial_dims>(data_format, 'W'));
  DimensionHandle in_depth_dim = c->Dim(
      input_shape, GetTensorDimIndex<num_spatial_dims>(data_format, 'C'));

  Padding padding;
  TF_RETURN_IF_ERROR(c->GetAttr("padding", &padding));

  std::vector<int64_t> explicit_paddings;
  if (supports_explicit_padding) {
    Status status = c->GetAttr("explicit_paddings", &explicit_paddings);
    // Use the default value, which is an empty list, if the attribute is not
    // found. Otherwise return the error to the caller.
    if (!status.ok() && !errors::IsNotFound(status)) {
      return status;
    }
    TF_RETURN_IF_ERROR(CheckValidPadding(padding, explicit_paddings,
                                         /*num_dims=*/4, data_format));
  } else {
    DCHECK(padding != Padding::EXPLICIT);
  }

  ShapeHandle output_shape;
  DimensionHandle output_rows, output_cols, output_depth;
  int64_t pad_rows_before = -1, pad_rows_after = -1;
  int64_t pad_cols_before = -1, pad_cols_after = -1;
  if (padding == Padding::EXPLICIT) {
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'H',
                             &pad_rows_before, &pad_rows_after);
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'W',
                             &pad_cols_before, &pad_cols_after);
  }
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_rows_dim, kernel_rows, /*dilation_rate=*/1, stride_rows, padding,
      pad_rows_before, pad_rows_after, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_cols_dim, kernel_cols, /*dilation_rate=*/1, stride_cols, padding,
      pad_cols_before, pad_cols_after, &output_cols));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_depth_dim, kernel_depth, /*dilation_rate=*/1, stride_depth, padding,
      /*pad_before*/ 0, /*pad_after*/ 0, &output_depth));

  TF_RETURN_IF_ERROR(MakeShapeFromFormat(data_format, batch_size_dim,
                                         {output_rows, output_cols},
                                         output_depth, &output_shape, c));

  c->set_output(0, output_shape);
  return Status::OK();
}