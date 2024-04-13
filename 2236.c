  bool MaybeTensorValueToShape(InferenceContext* ic, const Tensor& tensor,
                               ShapeHandle* tensors_as_shapes) {
    // Integer tensors of rank one can also be interpreted as a shape
    // provided all their values are >= -1.

    if (IsIntegerVector(tensor)) {
      bool has_values_smaller_than_minus_1 = false;
      std::vector<DimensionHandle> dims;
      for (int i = 0; i < tensor.NumElements(); i++) {
        int64_t value = tensor.dtype() == DT_INT32 ? tensor.flat<int32>()(i)
                                                   : tensor.flat<int64_t>()(i);
        has_values_smaller_than_minus_1 |= (value < -1);
        // Mark this as UnknownDim from Const.
        dims.push_back(value < 0 ? ic->MakeDim(kUnknownDimFromConst)
                                 : ic->MakeDim(value));
      }

      if (!has_values_smaller_than_minus_1) {
        *tensors_as_shapes = ic->MakeShape(dims);
        return true;
      }
    } else if (IsIntegerScalar(tensor)) {
      // Scalar constant.
      int64_t value = tensor.dtype() == DT_INT32 ? tensor.flat<int32>()(0)
                                                 : tensor.flat<int64_t>()(0);
      if (value == -1) {
        // Scalar value -1 represents an unknown shape. If we would try to
        // MakeShape(MakeDim) with it, we would get vector of unknown size.
        *tensors_as_shapes = ic->UnknownShape();
        return true;
      } else if (value >= 0) {
        // Ideally, values can be < -1, but MakeDim() fails with a value < -1.
        // It's a limitation as we use ShapeHandle as a means to pass values.
        *tensors_as_shapes = ic->MakeShape({ic->MakeDim(value)});
        return true;
      }
    }
    return false;
  }