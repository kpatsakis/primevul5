Status GatherNdShape(InferenceContext* c) {
  ShapeHandle params;
  std::vector<ShapeAndType> handle_shape_and_type;
  if (c->input_handle_shapes_and_types(0) != nullptr) {
    TF_RETURN_IF_ERROR(
        ValidateVariableResourceHandle(c, &handle_shape_and_type));
    params = handle_shape_and_type[0].shape;
  } else {
    params = c->input(0);
  }
  ShapeHandle indices;
  TF_RETURN_IF_ERROR(c->WithRankAtLeast(c->input(1), 1, &indices));
  DimensionHandle r_dim = c->Dim(indices, -1);

  if (!c->RankKnown(params) || !c->ValueKnown(r_dim)) {
    c->set_output(0, c->UnknownShape());
    return Status::OK();
  }

  if (c->Value(r_dim) > c->Rank(params)) {
    return errors::InvalidArgument(
        "indices.shape[-1] must be <= params.rank, but saw indices shape: ",
        c->DebugString(indices), " and params shape: ", c->DebugString(params));
  }

  // Remove r_dim from indices to get output.
  ShapeHandle indices_slice;
  ShapeHandle params_slice;
  TF_RETURN_IF_ERROR(c->Subshape(indices, 0, -1, &indices_slice));
  TF_RETURN_IF_ERROR(c->Subshape(params, c->Value(r_dim), &params_slice));
  ShapeHandle out;
  TF_RETURN_IF_ERROR(c->Concatenate(indices_slice, params_slice, &out));
  c->set_output(0, out);
  return Status::OK();
}