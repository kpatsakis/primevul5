Status MatrixSetDiagV2Shape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape, diag_shape, diag_index_shape;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 2, &input_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(1), 1, &diag_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtMost(c->input(2), 1, &diag_index_shape));

  int32_t lower_diag_index = 0;
  int32_t upper_diag_index = 0;
  bool diag_index_known = false;
  const Tensor* diag_index_tensor = c->input_tensor(2);
  if (diag_index_tensor != nullptr && c->FullyDefined(diag_index_shape)) {
    diag_index_known = true;
    TF_RETURN_IF_ERROR(ReadDiagIndex(c, diag_index_tensor, &lower_diag_index,
                                     &upper_diag_index));
    if (lower_diag_index > upper_diag_index) {
      return errors::InvalidArgument(
          "lower_diag_index is greater than upper_diag_index");
    }
  }

  // Do more checks when input rank is known.
  if (c->RankKnown(input_shape)) {
    int32_t input_rank = c->Rank(input_shape);

    // If diag_index is set, we know the exact rank of diagonal.
    if (diag_index_known) {
      TF_RETURN_IF_ERROR(c->WithRank(
          c->input(1),
          (lower_diag_index == upper_diag_index) ? input_rank - 1 : input_rank,
          &diag_shape));
    } else {
      TF_RETURN_IF_ERROR(
          c->WithRankAtLeast(c->input(1), input_rank - 1, &diag_shape));
      TF_RETURN_IF_ERROR(
          c->WithRankAtMost(c->input(1), input_rank, &diag_shape));
    }

    // Validates lower_diag_index and upper_diag_index.
    const int32_t num_rows = c->Value(c->Dim(input_shape, input_rank - 2));
    const int32_t num_cols = c->Value(c->Dim(input_shape, input_rank - 1));
    if (num_rows != InferenceContext::kUnknownDim &&
        num_cols != InferenceContext::kUnknownDim) {
      if (lower_diag_index != 0 &&  // For when num_rows or num_cols == 0.
          (-num_rows >= lower_diag_index || lower_diag_index >= num_cols)) {
        return errors::InvalidArgument("lower_diag_index is out of bound.");
      }
      if (upper_diag_index != 0 &&  // For when num_rows or num_cols == 0.
          (-num_rows >= upper_diag_index || upper_diag_index >= num_cols)) {
        return errors::InvalidArgument("upper_diag_index is out of bound.");
      }
    }
  }

  ShapeHandle output_shape = input_shape;
  if (c->RankKnown(diag_shape) && !c->FullyDefined(input_shape)) {
    // Try to infer parts of shape from diag.
    ShapeHandle diag_prefix;
    TF_RETURN_IF_ERROR(c->Subshape(
        diag_shape, 0, (lower_diag_index == upper_diag_index) ? -1 : -2,
        &diag_prefix));

    // The inner matrices can be rectangular, so we can't pinpoint their
    // exact height and width by just lower_diag_index, upper_diag_index,
    // and the longest length of given diagonals.
    TF_RETURN_IF_ERROR(
        c->Concatenate(diag_prefix, c->UnknownShapeOfRank(2), &diag_shape));
    TF_RETURN_IF_ERROR(c->Merge(input_shape, diag_shape, &output_shape));
  }
  c->set_output(0, output_shape);
  return Status::OK();
}