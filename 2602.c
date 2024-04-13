Status MatrixDiagPartV2Shape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape, diag_index_shape, unused_shape;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 2, &input_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtMost(c->input(1), 1, &diag_index_shape));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(2), 0, &unused_shape));

  const Tensor* diag_index_tensor = c->input_tensor(1);
  if (!c->RankKnown(input_shape) || !c->FullyDefined(diag_index_shape) ||
      diag_index_tensor == nullptr) {
    c->set_output(0, c->UnknownShape());
    return Status::OK();
  }
  int32_t lower_diag_index = 0;
  int32_t upper_diag_index = 0;
  TF_RETURN_IF_ERROR(ReadDiagIndex(c, diag_index_tensor, &lower_diag_index,
                                   &upper_diag_index));
  if (lower_diag_index > upper_diag_index) {
    return errors::InvalidArgument(
        "lower_diag_index is greater than upper_diag_index");
  }

  // Validates lower_diag_index and upper_diag_index.
  const int32_t input_rank = c->Rank(input_shape);
  const int32_t num_rows = c->Value(c->Dim(input_shape, input_rank - 2));
  const int32_t num_cols = c->Value(c->Dim(input_shape, input_rank - 1));
  int32_t max_diag_len = InferenceContext::kUnknownDim;
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
    max_diag_len = std::min(num_rows + std::min(upper_diag_index, 0),
                            num_cols - std::max(lower_diag_index, 0));
  }

  std::vector<DimensionHandle> dims;
  dims.reserve(input_rank - 2);
  for (int i = 0; i < input_rank - 2; ++i) {
    dims.push_back(c->Dim(input_shape, i));
  }
  if (lower_diag_index < upper_diag_index) {
    dims.push_back(c->MakeDim(upper_diag_index - lower_diag_index + 1));
  }
  dims.push_back(c->MakeDim(max_diag_len));
  c->set_output(0, c->MakeShape(dims));
  return Status::OK();
}