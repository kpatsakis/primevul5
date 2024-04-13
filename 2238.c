  bool IsIntegerVector(const Tensor& tensor) {
    if (tensor.dims() == 1 &&
        (tensor.dtype() == DT_INT32 || tensor.dtype() == DT_INT64)) {
      return true;
    }
    return false;
  }