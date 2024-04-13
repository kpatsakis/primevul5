Status ScatterNdShapeHelper(InferenceContext* c, ShapeHandle indices_shape,
                            ShapeHandle updates_shape,
                            ShapeHandle input_shape) {
  if (c->Value(c->NumElements(input_shape)) == 0 &&
      (c->Value(c->NumElements(indices_shape)) > 0 ||
       c->Value(c->NumElements(updates_shape)) > 0)) {
    return errors::InvalidArgument(
        "Indices and updates specified for empty input");
  }

  if (c->RankKnown(indices_shape) && c->RankKnown(updates_shape)) {
    const int64_t outer_dims = c->Rank(indices_shape) - 1;
    const DimensionHandle ixdim = c->Dim(indices_shape, -1);

    // We can only do more validation if the last dimension of indices
    // is a known value.
    if (c->ValueKnown(ixdim)) {
      int64_t ix = c->Value(ixdim);
      ShapeHandle unused;
      ShapeHandle prefix_indices;
      TF_RETURN_IF_ERROR(
          c->Subshape(indices_shape, 0, outer_dims, &prefix_indices));
      ShapeHandle prefix_updates;
      TF_RETURN_IF_ERROR(
          c->Subshape(updates_shape, 0, outer_dims, &prefix_updates));

      Status s = c->Merge(prefix_indices, prefix_updates, &unused);
      if (!s.ok()) {
        return errors::InvalidArgument(
            "Dimensions [0,", outer_dims,
            ") of indices[shape=", c->DebugString(indices_shape),
            "] = ", c->DebugString(prefix_indices),
            " must match dimensions [0,", outer_dims,
            ") of updates[shape=", c->DebugString(updates_shape),
            "] = ", c->DebugString(prefix_updates), ": ", s.error_message());
      }

      ShapeHandle suffix_output;
      TF_RETURN_IF_ERROR(c->Subshape(input_shape, ix, &suffix_output));
      ShapeHandle suffix_updates;
      TF_RETURN_IF_ERROR(
          c->Subshape(updates_shape, outer_dims, &suffix_updates));
      s = c->Merge(suffix_output, suffix_updates, &unused);
      if (!s.ok()) {
        return errors::InvalidArgument(
            "Dimensions [", ix, ",", c->Rank(input_shape),
            ") of input[shape=", c->DebugString(input_shape),
            "] = ", c->DebugString(suffix_output), " must match dimensions [",
            outer_dims, ",", c->Rank(updates_shape),
            ") of updates[shape=", c->DebugString(updates_shape),
            "] = ", c->DebugString(suffix_updates), ": ", s.error_message());
      }
    }
  }

  if (c->input_handle_shapes_and_types(0) == nullptr && c->num_outputs() > 0) {
    // This is called for tf.scatter_nd; output is a tensor with this shape.
    c->set_output(0, input_shape);
  }
  return Status::OK();
}