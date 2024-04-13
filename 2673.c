int64_t OpLevelCostEstimator::CalculateTensorElementCount(
    const OpInfo::TensorProperties& tensor, bool* found_unknown_shapes) {
  VLOG(2) << "   with " << DataTypeString(tensor.dtype()) << " tensor of shape "
          << tensor.shape().DebugString();
  int64_t tensor_size = 1;
  int num_dims = std::max(1, tensor.shape().dim_size());
  auto tensor_shape =
      MaybeGetMinimumShape(tensor.shape(), num_dims, found_unknown_shapes);
  for (const auto& dim : tensor_shape.dim()) {
    tensor_size *= dim.size();
  }
  return tensor_size;
}