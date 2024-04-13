Status BatchMatMulShape(shape_inference::InferenceContext* c) {
  ShapeHandle a_shape;
  ShapeHandle b_shape;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 2, &a_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(1), 2, &b_shape));

  // Determine output rows and cols.
  bool adj_x;
  bool adj_y;
  TF_RETURN_IF_ERROR(c->GetAttr("adj_x", &adj_x));
  TF_RETURN_IF_ERROR(c->GetAttr("adj_y", &adj_y));
  DimensionHandle output_rows = c->Dim(a_shape, adj_x ? -1 : -2);
  DimensionHandle output_cols = c->Dim(b_shape, adj_y ? -2 : -1);

  // Batch dims match between inputs.
  ShapeHandle a_batch_dims;
  ShapeHandle b_batch_dims;
  ShapeHandle batch_dims;
  TF_RETURN_IF_ERROR(c->Subshape(a_shape, 0, -2, &a_batch_dims));
  TF_RETURN_IF_ERROR(c->Subshape(b_shape, 0, -2, &b_batch_dims));
  TF_RETURN_IF_ERROR(c->Merge(a_batch_dims, b_batch_dims, &batch_dims));

  // Assert inner dims match.
  DimensionHandle unused;
  TF_RETURN_IF_ERROR(c->Merge(c->Dim(a_shape, adj_x ? -2 : -1),
                              c->Dim(b_shape, adj_y ? -1 : -2), &unused));

  ShapeHandle out;
  TF_RETURN_IF_ERROR(
      c->Concatenate(batch_dims, c->Matrix(output_rows, output_cols), &out));
  c->set_output(0, out);
  return Status::OK();
}