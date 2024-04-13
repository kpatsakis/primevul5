Status ValidateSparseTensor(InferenceContext* c, ShapeHandle indices_shape,
                            ShapeHandle values_shape, ShapeHandle shape_shape) {
  // Validate ranks.
  ShapeHandle unused_shape;
  TF_RETURN_IF_ERROR(c->WithRank(indices_shape, 2, &unused_shape));
  TF_RETURN_IF_ERROR(c->WithRank(values_shape, 1, &unused_shape));
  TF_RETURN_IF_ERROR(c->WithRank(shape_shape, 1, &unused_shape));

  // Number of elements in indices and values must match.
  DimensionHandle num_index_elements_dim = c->Dim(indices_shape, 0);
  if (c->ValueKnown(num_index_elements_dim)) {
    DimensionHandle num_values_elements_dim = c->Dim(values_shape, 0);
    if (c->ValueKnown(num_values_elements_dim)) {
      int64_t num_index_elements = c->Value(num_index_elements_dim);
      int64_t num_values_elements = c->Value(num_values_elements_dim);
      if (num_index_elements != num_values_elements) {
        return errors::InvalidArgument("Number of elements in index (",
                                       num_index_elements, ") and values (",
                                       num_values_elements, ") do not match.");
      }
    }
  }

  // Rank embedded in indices must match shape.
  DimensionHandle index_rank_dim = c->Dim(indices_shape, 1);
  if (c->ValueKnown(index_rank_dim)) {
    DimensionHandle shape_rank_dim = c->Dim(shape_shape, 0);
    if (c->ValueKnown(shape_rank_dim)) {
      int64_t index_rank = c->Value(index_rank_dim);
      int32_t shape_rank = c->Value(shape_rank_dim);
      if (index_rank != shape_rank) {
        return errors::InvalidArgument("Index rank (", index_rank,
                                       ") and shape rank (", shape_rank,
                                       ") do not match.");
      }
    }
  }

  return Status::OK();
}