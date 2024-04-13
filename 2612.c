Status ReductionShape(InferenceContext* c) {
  ShapeHandle input = c->input(0);

  ShapeHandle indices;
  // Older versions of TensorFlow accidentally allowed higher rank tensors like
  // [[1,2]] or [[1],[2]] to represent axis=[1,2].
  if (c->graph_def_version() < 21) {
    indices = c->input(1);
  } else {
    TF_RETURN_IF_ERROR(c->WithRankAtMost(c->input(1), 1, &indices));
  }

  bool keep_dims;
  TF_RETURN_IF_ERROR(c->GetAttr("keep_dims", &keep_dims));

  const Tensor* reduction_indices_t = c->input_tensor(1);
  if (reduction_indices_t == nullptr || !c->RankKnown(input)) {
    // If we do not have the reduction values at runtime, or the
    // rank of the input, we don't know the output shape.

    if (keep_dims && c->RankKnown(input)) {
      // output rank matches input input if <keep_dims>.
      c->set_output(0, c->UnknownShapeOfRank(c->Rank(input)));
      return Status::OK();
    } else {
      return shape_inference::UnknownShape(c);
    }
  }

  const int32_t input_rank = c->Rank(input);
  std::set<int64_t> true_indices;
  if (reduction_indices_t->dtype() == DataType::DT_INT32) {
    TF_RETURN_IF_ERROR(ReductionShapeHelper<int32>(reduction_indices_t,
                                                   input_rank, &true_indices));
  } else if (reduction_indices_t->dtype() == DataType::DT_INT64) {
    TF_RETURN_IF_ERROR(ReductionShapeHelper<int64_t>(
        reduction_indices_t, input_rank, &true_indices));
  } else {
    return errors::InvalidArgument(
        "reduction_indices can only be int32 or int64");
  }

  std::vector<DimensionHandle> dims;
  for (int i = 0; i < input_rank; ++i) {
    if (true_indices.count(i) > 0) {
      if (keep_dims) {
        dims.emplace_back(c->MakeDim(1));
      }
    } else {
      dims.emplace_back(c->Dim(input, i));
    }
  }

  c->set_output(0, c->MakeShape(dims));
  return Status::OK();
}