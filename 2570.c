Status SliceHelper(InferenceContext* c, ShapeHandle begin_value,
                   const Tensor* sizes_value,
                   std::vector<DimensionHandle>* dims) {
  auto sizes_vec = sizes_value->vec<T>();
  for (int i = 0; i < sizes_value->NumElements(); ++i) {
    DimensionHandle dim = c->Dim(c->input(0), i);
    if (sizes_vec(i) != -1) {
      auto dim_val = c->Value(dim);
      if (sizes_vec(i) < 0) {
        return errors::InvalidArgument(
            "Out of bounds slicing on dimension ", i, " of length ", dim_val,
            ": sizes vector cannot be < -1, but was ", sizes_vec(i));
      }

      dims->emplace_back(c->MakeDim(sizes_vec(i)));
    } else {
      DimensionHandle result;
      TF_RETURN_IF_ERROR(c->Subtract(dim, c->Dim(begin_value, i), &result));
      dims->emplace_back(result);
    }
  }

  return Status::OK();
}