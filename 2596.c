Status DepthwiseConv2DNativeShapeImpl(shape_inference::InferenceContext* c,
                                      bool supports_explicit_padding) {
  ShapeHandle input_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 4, &input_shape));
  ShapeHandle filter_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 4, &filter_shape));

  std::vector<int32> strides;
  TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));

  if (strides.size() != 4) {
    return errors::InvalidArgument(
        "DepthwiseConv2D requires the stride attribute to contain 4 values, "
        "but got: ",
        strides.size());
  }

  std::vector<int32> dilations;
  if (!c->GetAttr("dilations", &dilations).ok()) {
    dilations.resize(4, 1);
  }

  if (dilations.size() != 4) {
    return errors::InvalidArgument(
        "DepthwiseConv2D requires the dilations attribute to contain 4 values, "
        "but got: ",
        dilations.size());
  }

  string data_format_str;
  Status s = c->GetAttr("data_format", &data_format_str);
  TensorFormat data_format;
  if (!s.ok() || !FormatFromString(data_format_str, &data_format)) {
    data_format = FORMAT_NHWC;
  }
  int32_t stride_rows;
  int32_t stride_cols;
  int32_t dilation_rows;
  int32_t dilation_cols;
  if (data_format == FORMAT_NCHW) {
    // Canonicalize input shape to NHWC so the shape inference code below can
    // process it.
    input_shape =
        c->MakeShape({{c->Dim(input_shape, 0), c->Dim(input_shape, 2),
                       c->Dim(input_shape, 3), c->Dim(input_shape, 1)}});
    stride_rows = strides[2];
    stride_cols = strides[3];
    dilation_rows = dilations[2];
    dilation_cols = dilations[3];
  } else {
    stride_rows = strides[1];
    stride_cols = strides[2];
    dilation_rows = dilations[1];
    dilation_cols = dilations[2];
  }

  DimensionHandle batch_size_dim = c->Dim(input_shape, 0);
  DimensionHandle in_rows_dim = c->Dim(input_shape, 1);
  DimensionHandle in_cols_dim = c->Dim(input_shape, 2);

  DimensionHandle filter_rows_dim = c->Dim(filter_shape, 0);
  DimensionHandle filter_cols_dim = c->Dim(filter_shape, 1);
  DimensionHandle input_depth = c->Dim(filter_shape, 2);
  DimensionHandle depth_multiplier = c->Dim(filter_shape, 3);

  // Check that the input depths are compatible.
  TF_RETURN_IF_ERROR(
      c->Merge(c->Dim(input_shape, 3), input_depth, &input_depth));

  DimensionHandle output_depth;
  TF_RETURN_IF_ERROR(c->Multiply(input_depth, depth_multiplier, &output_depth));

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

  // TODO(mrry,shlens): Raise an error if the stride would cause
  // information in the input to be ignored. This will require a change
  // in the kernel implementation.
  DimensionHandle output_rows, output_cols;
  int64_t pad_rows_before = -1, pad_rows_after = -1;
  int64_t pad_cols_before = -1, pad_cols_after = -1;
  if (padding == Padding::EXPLICIT) {
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'H',
                             &pad_rows_before, &pad_rows_after);
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'W',
                             &pad_cols_before, &pad_cols_after);
  }
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_rows_dim, filter_rows_dim, dilation_rows, stride_rows, padding,
      pad_rows_before, pad_rows_after, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_cols_dim, filter_cols_dim, dilation_cols, stride_cols, padding,
      pad_cols_before, pad_cols_after, &output_cols));

  ShapeHandle output_shape;
  if (data_format == FORMAT_NCHW) {
    output_shape =
        c->MakeShape({batch_size_dim, output_depth, output_rows, output_cols});
  } else {
    output_shape =
        c->MakeShape({batch_size_dim, output_rows, output_cols, output_depth});
  }
  c->set_output(0, output_shape);
  return Status::OK();
}