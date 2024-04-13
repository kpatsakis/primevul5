Status ReadDiagIndex(InferenceContext* c, const Tensor* diag_index_tensor,
                     int32* lower_diag_index, int32* upper_diag_index) {
  // This function assumes that the shape of diag_index_tensor is fully defined.
  if (diag_index_tensor->dims() == 0) {
    *lower_diag_index = diag_index_tensor->scalar<int32>()();
    *upper_diag_index = *lower_diag_index;
  } else {
    int32_t num_elements = diag_index_tensor->dim_size(0);
    if (num_elements == 1) {
      *lower_diag_index = diag_index_tensor->vec<int32>()(0);
      *upper_diag_index = *lower_diag_index;
    } else if (num_elements == 2) {
      *lower_diag_index = diag_index_tensor->vec<int32>()(0);
      *upper_diag_index = diag_index_tensor->vec<int32>()(1);
    } else {
      return errors::InvalidArgument(
          "diag_index must be a vector with one or two elements. It has ",
          num_elements, " elements.");
    }
  }
  return Status::OK();
}