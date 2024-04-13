Status SliceShape(InferenceContext* c) {
  ShapeHandle input = c->input(0);
  ShapeHandle begin_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 1, &begin_shape));
  ShapeHandle sizes_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(2), 1, &sizes_shape));

  // Merge to check compatibility of begin and sizes tensors.
  TF_RETURN_IF_ERROR(c->Merge(begin_shape, sizes_shape, &begin_shape));

  DimensionHandle ndims = c->Dim(begin_shape, 0);
  if (c->ValueKnown(ndims)) {
    TF_RETURN_IF_ERROR(c->WithRank(input, c->Value(ndims), &input));
  }

  // NOTE(mrry): Use MakeShapeFromShapeTensor to handle partially-known
  // values, even though the `begin` value does not represent a shape.
  ShapeHandle begin_value;
  TF_RETURN_IF_ERROR(c->MakeShapeFromShapeTensor(1, &begin_value));

  // We check the tensor value here and will only use
  // `MakeShapeFromShapeTensor` when `sizes_value` is null.
  // The reason is that `sizes` might contain -1, which can't
  // be represented (-1 in the ShapeHandle would mean "unknown").
  const Tensor* sizes_value = c->input_tensor(2);

  if (sizes_value != nullptr) {
    TF_RETURN_IF_ERROR(
        c->WithRank(begin_value, sizes_value->NumElements(), &begin_value));
    std::vector<DimensionHandle> dims;
    // If the begin and sizes tensors are available, then
    // we can be precise about the shape of the output.
    if (sizes_value->dtype() == DT_INT64) {
      TF_RETURN_IF_ERROR(
          SliceHelper<int64_t>(c, begin_value, sizes_value, &dims));
    } else {
      TF_RETURN_IF_ERROR(
          SliceHelper<int32>(c, begin_value, sizes_value, &dims));
    }
    c->set_output(0, c->MakeShape(dims));
    return Status::OK();
  } else {
    // In case `sizes` is not available (`sizes_value` is null),
    // we could try to use `MakeShapeFromShapeTensor` here.
    // If sizes contain -1, we will simply consider it as `Unknown`.
    // This is less than ideal but still an improvement of shape inference.
    // The following is an example that returns [None, 1, None] with this
    // code path:
    //   z = tf.zeros((1, 2, 3))
    //   m = tf.slice(z, [0, 0, 0], [tf.constant(1) + 0, 1, -1])
    //   m.get_shape().as_list()
    ShapeHandle sizes_value;
    TF_RETURN_IF_ERROR(c->MakeShapeFromShapeTensor(2, &sizes_value));
    if (c->RankKnown(sizes_value)) {
      TF_RETURN_IF_ERROR(
          c->WithRank(begin_value, c->Rank(sizes_value), &begin_value));
      std::vector<DimensionHandle> dims;
      dims.reserve(c->Rank(sizes_value));
      for (int i = 0; i < c->Rank(sizes_value); ++i) {
        dims.emplace_back(c->Dim(sizes_value, i));
      }
      c->set_output(0, c->MakeShape(dims));
      return Status::OK();
    }
    // We might know the rank of the input.
    if (c->RankKnown(input)) {
      c->set_output(0, c->UnknownShapeOfRank(c->Rank(input)));
      return Status::OK();
    } else {
      return shape_inference::UnknownShape(c);
    }
  }

  return Status::OK();
}