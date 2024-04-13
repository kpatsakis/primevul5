Status BatchMatMulV2Shape(shape_inference::InferenceContext* c) {
  ShapeHandle a_shape;
  ShapeHandle b_shape;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(0), 2, &a_shape));
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(1), 2, &b_shape));

  // Determine output rows and columns.
  bool adj_x;
  bool adj_y;
  TF_RETURN_IF_ERROR(c->GetAttr("adj_x", &adj_x));
  TF_RETURN_IF_ERROR(c->GetAttr("adj_y", &adj_y));
  DimensionHandle output_rows = c->Dim(a_shape, adj_x ? -1 : -2);
  DimensionHandle output_cols = c->Dim(b_shape, adj_y ? -2 : -1);

  // Inner dimensions should be compatible.
  DimensionHandle inner_merged;
  TF_RETURN_IF_ERROR(c->Merge(c->Dim(a_shape, adj_x ? -2 : -1),
                              c->Dim(b_shape, adj_y ? -1 : -2), &inner_merged));

  // Batch dimensions should broadcast with each other.
  ShapeHandle a_batch_shape;
  ShapeHandle b_batch_shape;
  ShapeHandle output_batch_shape;
  TF_RETURN_IF_ERROR(c->Subshape(a_shape, 0, -2, &a_batch_shape));
  TF_RETURN_IF_ERROR(c->Subshape(b_shape, 0, -2, &b_batch_shape));

  TF_RETURN_IF_ERROR(BroadcastBinaryOpOutputShapeFnHelper(
      c, a_batch_shape, b_batch_shape, true, &output_batch_shape));

  ShapeHandle output_shape;
  TF_RETURN_IF_ERROR(c->Concatenate(
      output_batch_shape, c->Matrix(output_rows, output_cols), &output_shape));

  c->set_output(0, output_shape);
  return Status::OK();
}