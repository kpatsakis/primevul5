Status MatrixDiagV2Shape(shape_inference::InferenceContext* c) {
  // Checks input ranks.
  ShapeHandle input_shape, diag_index_shape, unused_shape;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 1, &input_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtMost(c->input(1), 1, &diag_index_shape));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(2), 0, &unused_shape));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(3), 0, &unused_shape));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(4), 0, &unused_shape));

  // Reads the diagonal indices.
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

  // Checks if the number of diagonals provided matches what we imply from
  // lower_diag_index and upper_diag_index.
  const int32_t input_rank = c->Rank(input_shape);
  if (lower_diag_index < upper_diag_index) {
    const int32_t num_diags = c->Value(c->Dim(input_shape, input_rank - 2));
    const int32_t other_dim = c->Value(c->Dim(input_shape, input_rank - 1));

    if (num_diags != (upper_diag_index - lower_diag_index + 1)) {
      return errors::InvalidArgument(
          "The number of rows of `diagonal` doesn't match the number of "
          "diagonals implied from `d_lower` and `d_upper`.\n",
          "num_diags = ", num_diags, ", d_lower = ", lower_diag_index,
          ", d_upper = ", upper_diag_index, " ", input_rank, " ", other_dim);
    }
  }

  // Reads num_rows and num_cols.
  const Tensor* num_rows_tensor = c->input_tensor(2);
  const Tensor* num_cols_tensor = c->input_tensor(3);
  int64_t num_rows = -1;
  int64_t num_cols = -1;
  if (num_rows_tensor != nullptr) {
    TF_RETURN_IF_ERROR(c->GetScalarFromTensor(num_rows_tensor, &num_rows));
  }
  if (num_cols_tensor != nullptr) {
    TF_RETURN_IF_ERROR(c->GetScalarFromTensor(num_cols_tensor, &num_cols));
  }

  // Infers the missing num_rows or num_cols: If both are missing, assume
  // output is square. Otherwise, use the smallest possible value. Also
  // validates the provided values.
  const int32_t max_diag_len = c->Value(c->Dim(input_shape, input_rank - 1));
  const int32_t min_num_rows = max_diag_len - std::min(upper_diag_index, 0);
  const int32_t min_num_cols = max_diag_len + std::max(lower_diag_index, 0);
  if (num_rows == -1 && num_cols == -1) {  // Special case.
    num_rows = std::max(min_num_rows, min_num_cols);
    num_cols = num_rows;
  }
  if (num_rows == -1) {
    num_rows = min_num_rows;
  } else if (num_rows < min_num_rows) {
    return errors::InvalidArgument("num_rows is too small");
  }
  if (num_cols == -1) {
    num_cols = min_num_cols;
  } else if (num_cols < min_num_cols) {
    return errors::InvalidArgument("num_cols is too small.");
  }
  // At least one of them must match the minimum length.
  if (num_rows != min_num_rows && num_cols != min_num_cols) {
    return errors::InvalidArgument(
        "num_rows and num_cols are not consistent with lower_diag_index, "
        "upper_diag_index, and the length of the given diagonals.\n",
        "num_rows = ", num_rows, " != min_num_rows = ", min_num_rows,
        ", num_cols = ", num_cols, " != min_num_cols = ", min_num_cols);
  }

  // Sets output shape.
  ShapeHandle output_shape;
  const DimensionHandle output_row_dim = c->MakeDim(num_rows);
  const DimensionHandle output_col_dim = c->MakeDim(num_cols);
  if (lower_diag_index == upper_diag_index) {
    TF_RETURN_IF_ERROR(c->ReplaceDim(input_shape, input_rank - 1,
                                     output_row_dim, &output_shape));
    TF_RETURN_IF_ERROR(
        c->Concatenate(output_shape, c->Vector(output_col_dim), &output_shape));
  } else {
    TF_RETURN_IF_ERROR(c->ReplaceDim(input_shape, input_rank - 2,
                                     output_row_dim, &output_shape));
    TF_RETURN_IF_ERROR(c->ReplaceDim(output_shape, input_rank - 1,
                                     output_col_dim, &output_shape));
  }
  c->set_output(0, output_shape);
  return Status::OK();
}