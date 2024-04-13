Status BroadcastBinaryOpOutputShapeFnHelper(InferenceContext* c,
                                            ShapeHandle shape_x,
                                            ShapeHandle shape_y,
                                            bool incompatible_shape_error,
                                            ShapeHandle* out) {
  CHECK_NOTNULL(out);
  if (!c->RankKnown(shape_x) || !c->RankKnown(shape_y)) {
    *out = c->UnknownShape();
    return Status::OK();
  }
  const int32_t rank_x = c->Rank(shape_x);
  const int32_t rank_y = c->Rank(shape_y);
  const int32_t rank_out = std::max(rank_x, rank_y);

  // To compute the broadcast dimensions, we zip together shape_x and shape_y
  // and
  // pad with 1 to make them the same length.
  std::vector<DimensionHandle> dims;
  DimensionHandle dim_one;
  if (rank_x != rank_y) dim_one = c->MakeDim(1);
  for (int i = 0; i < rank_out; ++i) {
    const auto dim_x = i < (rank_out - rank_x)
                           ? dim_one
                           : c->Dim(shape_x, i - (rank_out - rank_x));
    const bool dim_y_is_one = (i < (rank_out - rank_y));
    const auto dim_y =
        dim_y_is_one ? dim_one : c->Dim(shape_y, i - (rank_out - rank_y));
    if (!c->ValueKnown(dim_x) || !c->ValueKnown(dim_y)) {
      // One or both dimensions is unknown.
      //
      // - If either dimension is greater than 1, we assume that the program is
      // correct, and the other dimension will be broadcast to match it.
      // TODO(cwhipkey): For shape inference, if we eliminate the shape checks
      // in C++ op code, we must still assert that the unknown dim is either 1
      // or the same as the known dim.
      // - If either dimension is 1, the other dimension is the output.
      // - If both are unknown then dimension is unknown
      if (c->Value(dim_x) > 1) {
        if (!incompatible_shape_error) {
          *out = c->UnknownShape();
          return Status::OK();
        }
        dims.push_back(dim_x);
      } else if (c->Value(dim_y) > 1) {
        if (!incompatible_shape_error) {
          *out = c->UnknownShape();
          return Status::OK();
        }
        dims.push_back(dim_y);
      } else if (c->Value(dim_x) == 1) {
        dims.push_back(dim_y);
      } else if (c->Value(dim_y) == 1) {
        dims.push_back(dim_x);
      } else if (dim_y.SameHandle(dim_x)) {
        dims.push_back(dim_x);
      } else if (!c->ValueKnown(dim_x) && !c->ValueKnown(dim_y)) {
        dims.push_back(c->UnknownDim());
      } else {
        if (!incompatible_shape_error) {
          *out = c->UnknownShape();
          return Status::OK();
        }
        dims.push_back(c->UnknownDim());
      }
    } else if (c->Value(dim_x) == 1 || c->Value(dim_y) == 1) {
      if (c->Value(dim_x) == 1 && !dim_y_is_one) {
        // We will broadcast dim_x to dim_y.
        dims.push_back(dim_y);
      } else {
        DCHECK_EQ(c->Value(dim_y), 1);
        // We will broadcast dim_y to dim_x.
        dims.push_back(dim_x);
      }
    } else {
      DimensionHandle dim;
      Status s = c->Merge(dim_x, dim_y, &dim);
      if (!s.ok()) {
        if (!incompatible_shape_error) {
          *out = c->MakeShape({});
          return Status::OK();
        }
        return s;
      }
      dims.push_back(dim);
    }
  }

  *out = c->MakeShape(dims);
  return Status::OK();
}