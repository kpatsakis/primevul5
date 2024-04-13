Status ReductionShapeHelper(const Tensor* reduction_indices_t,
                            const int32_t input_rank,
                            std::set<int64_t>* true_indices) {
  auto reduction_indices = reduction_indices_t->flat<T>();
  for (int i = 0; i < reduction_indices_t->NumElements(); ++i) {
    const T reduction_index = reduction_indices(i);
    if (reduction_index < -input_rank || reduction_index >= input_rank) {
      return errors::InvalidArgument("Invalid reduction dimension ",
                                     reduction_index, " for input with ",
                                     input_rank, " dimensions.");
    }

    auto wrapped_index = reduction_index;
    if (wrapped_index < 0) {
      wrapped_index += input_rank;
    }

    true_indices->insert(wrapped_index);
  }
  return Status::OK();
}