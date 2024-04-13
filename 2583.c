Status Pool3DShape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 5, &input_shape));

  string data_format;
  Status s = c->GetAttr("data_format", &data_format);

  std::vector<int32> strides;
  TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));
  if (strides.size() != 5) {
    return errors::InvalidArgument(
        "Pool3D ops require the stride attribute to contain 5 values, but "
        "got: ",
        strides.size());
  }

  std::vector<int32> kernel_sizes;
  TF_RETURN_IF_ERROR(c->GetAttr("ksize", &kernel_sizes));
  if (kernel_sizes.size() != 5) {
    return errors::InvalidArgument(
        "Pool3D requires the ksize attribute to contain 5 values, but got: ",
        kernel_sizes.size());
  }

  int32_t stride_planes, stride_rows, stride_cols;
  int32_t kernel_planes, kernel_rows, kernel_cols;

  if (s.ok() && data_format == "NCDHW") {
    // Convert input_shape to NDHWC.
    auto dim = [&](char dimension) {
      return c->Dim(input_shape, GetTensorDimIndex<3>(FORMAT_NCHW, dimension));
    };
    input_shape =
        c->MakeShape({{dim('N'), dim('0'), dim('1'), dim('2'), dim('C')}});
    stride_planes = strides[2];
    stride_rows = strides[3];
    stride_cols = strides[4];
    kernel_planes = kernel_sizes[2];
    kernel_rows = kernel_sizes[3];
    kernel_cols = kernel_sizes[4];
  } else {
    stride_planes = strides[1];
    stride_rows = strides[2];
    stride_cols = strides[3];
    kernel_planes = kernel_sizes[1];
    kernel_rows = kernel_sizes[2];
    kernel_cols = kernel_sizes[3];
  }

  DimensionHandle batch_size_dim = c->Dim(input_shape, 0);
  DimensionHandle in_planes_dim = c->Dim(input_shape, 1);
  DimensionHandle in_rows_dim = c->Dim(input_shape, 2);
  DimensionHandle in_cols_dim = c->Dim(input_shape, 3);
  DimensionHandle output_depth_dim = c->Dim(input_shape, 4);

  Padding padding;
  TF_RETURN_IF_ERROR(c->GetAttr("padding", &padding));

  // TODO(mrry,shlens): Raise an error if the stride would cause
  // information in the input to be ignored. This will require a change
  // in the kernel implementation.
  DimensionHandle output_planes, output_rows, output_cols;
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_planes_dim, kernel_planes, stride_planes, padding, &output_planes));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_rows_dim, kernel_rows, stride_rows, padding, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDims(
      c, in_cols_dim, kernel_cols, stride_cols, padding, &output_cols));

  ShapeHandle output_shape;
  if (data_format == "NCDHW") {
    output_shape = c->MakeShape({batch_size_dim, output_depth_dim,
                                 output_planes, output_rows, output_cols});
  } else {
    output_shape = c->MakeShape({batch_size_dim, output_planes, output_rows,
                                 output_cols, output_depth_dim});
  }

  c->set_output(0, output_shape);
  return Status::OK();
}