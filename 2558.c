Status MaxPoolV2Shape(shape_inference::InferenceContext* c, int num_inputs) {
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

  std::vector<int32> kernel_sizes;
  std::vector<int32> strides;

  if (c->num_inputs() + 2 == num_inputs) {
    TF_RETURN_IF_ERROR(c->GetAttr("ksize", &kernel_sizes));

    TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));
  } else {
    // Verify shape of ksize and strides input.
    ShapeHandle size;
    DimensionHandle unused;
    TF_RETURN_IF_ERROR(c->WithRank(c->input(c->num_inputs() - 2), 1, &size));
    TF_RETURN_IF_ERROR(c->WithValue(c->Dim(size, 0), 4, &unused));
    TF_RETURN_IF_ERROR(c->WithRank(c->input(c->num_inputs() - 1), 1, &size));
    TF_RETURN_IF_ERROR(c->WithValue(c->Dim(size, 0), 4, &unused));

    const Tensor* kernel_sizes_tensor = c->input_tensor(c->num_inputs() - 2);
    if (kernel_sizes_tensor == nullptr) {
      c->set_output(0, c->UnknownShape());
      return Status::OK();
    }
    kernel_sizes.resize(kernel_sizes_tensor->shape().num_elements());
    auto kernel_sizes_vec = kernel_sizes_tensor->flat<int32>();
    std::copy_n(&kernel_sizes_vec(0), kernel_sizes.size(),
                kernel_sizes.begin());

    const Tensor* strides_tensor = c->input_tensor(c->num_inputs() - 1);
    if (strides_tensor == nullptr) {
      c->set_output(0, c->UnknownShape());
      return Status::OK();
    }
    strides.resize(strides_tensor->shape().num_elements());
    auto strides_vec = strides_tensor->flat<int32>();
    std::copy_n(&strides_vec(0), strides.size(), strides.begin());
  }

  if (strides.size() != 4) {
    return errors::InvalidArgument(
        "MaxPool requires the stride attribute to contain 4 values, but "
        "got: ",
        strides.size());
  }
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

  ShapeHandle output_shape;
  DimensionHandle output_rows, output_cols, output_depth;
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_rows_dim, kernel_rows, stride_rows, padding, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_cols_dim, kernel_cols, stride_cols, padding, &output_cols));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_depth_dim, kernel_depth, stride_depth, padding, &output_depth));

  TF_RETURN_IF_ERROR(MakeShapeFromFormat(data_format, batch_size_dim,
                                         {output_rows, output_cols},
                                         output_depth, &output_shape, c));

  c->set_output(0, output_shape);
  return Status::OK();
}