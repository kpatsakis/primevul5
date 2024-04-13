Status BiasAddShape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape;

  // Fetch the data_format attribute, which may not exist.
  string data_format;
  Status s = c->GetAttr("data_format", &data_format);

  if (s.ok() && data_format == "NCHW") {
    TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 3, &input_shape));
  } else {
    TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 2, &input_shape));
  }

  ShapeHandle bias_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 1, &bias_shape));
  DimensionHandle bias_dim = c->Dim(bias_shape, 0);

  // If rank unknown, return unknown shape.
  if (!c->RankKnown(input_shape)) {
    c->set_output(0, c->UnknownShape());
    return Status::OK();
  }

  // Output has the same shape as the input, and matches the length of
  // the bias in its bias dimension.
  ShapeHandle output_shape;
  if (s.ok() && data_format == "NCHW") {
    // Merge the length of bias_shape into the third to last dimension
    ShapeHandle first;
    TF_RETURN_IF_ERROR(c->Subshape(input_shape, 0, 1, &first));

    ShapeHandle last;
    TF_RETURN_IF_ERROR(c->Subshape(input_shape, 2, &last));

    DimensionHandle input_bias_dim = c->Dim(input_shape, 1);
    DimensionHandle merged_bias_dim;
    TF_RETURN_IF_ERROR(c->Merge(input_bias_dim, bias_dim, &merged_bias_dim));
    ShapeHandle merged_bias = c->Vector(merged_bias_dim);

    ShapeHandle temp;
    TF_RETURN_IF_ERROR(c->Concatenate(first, merged_bias, &temp));
    TF_RETURN_IF_ERROR(c->Concatenate(temp, last, &output_shape));
  } else {
    ShapeHandle all_but_bias;
    TF_RETURN_IF_ERROR(c->Subshape(input_shape, 0, -1, &all_but_bias));

    DimensionHandle input_bias_dim = c->Dim(input_shape, -1);
    DimensionHandle merged_bias_dim;
    TF_RETURN_IF_ERROR(c->Merge(input_bias_dim, bias_dim, &merged_bias_dim));

    ShapeHandle merged_bias = c->Vector(merged_bias_dim);
    TF_RETURN_IF_ERROR(
        c->Concatenate(all_but_bias, merged_bias, &output_shape));
  }

  c->set_output(0, output_shape);
  return Status::OK();
}