Status ReduceScatterShape(shape_inference::InferenceContext* c) {
  shape_inference::ShapeHandle in = c->input(0);
  if (!c->RankKnown(in)) {
    // Input shape unknown, so set unknown output shape.
    c->set_output(0, in);
    return Status::OK();
  }

  shape_inference::ShapeHandle group_assignment_shape = c->input(1);
  if (c->Rank(group_assignment_shape) != 2)
    return errors::InvalidArgument(
        "ReduceScatter group_assignment should be rank 2");

  const Tensor* scatter_dimension = c->input_tensor(2);
  if (!scatter_dimension) {
    c->set_output(0, c->UnknownShape());
    return Status::OK();
  }
  int64_t scatter_dim;
  TF_RETURN_IF_ERROR(c->GetScalarFromTensor(scatter_dimension, &scatter_dim));

  std::vector<shape_inference::DimensionHandle> out_dims;
  out_dims.reserve(c->Rank(in));
  for (int i = 0; i < c->Rank(in); ++i) {
    // If the dimension is the scatter_dimension, then divide the dimension
    // by the partition size in the group_assignment.
    if (i == scatter_dim) {
      shape_inference::DimensionHandle dim = c->Dim(in, i);
      shape_inference::DimensionHandle out_dim;
      TF_RETURN_IF_ERROR(c->Divide(dim, c->Dim(group_assignment_shape, 1),
                                   /*evenly_divisible=*/true, &out_dim));
      out_dims.push_back(out_dim);
    } else {
      out_dims.emplace_back(c->Dim(in, i));
    }
  }
  c->set_output(0, c->MakeShape(out_dims));
  return Status::OK();
}