  bool IsIntegerScalar(const Tensor& tensor) {
    if (tensor.dims() == 0 &&
        (tensor.dtype() == DT_INT32 || tensor.dtype() == DT_INT64) &&
        tensor.NumElements() == 1) {
      return true;
    }
    return false;
  }