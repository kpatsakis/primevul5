bool IsTensorIdPortValid(const TensorId& tensor_id) {
  return tensor_id.index() >= Graph::kControlSlot;
}