  Status check_index_ordering(const Tensor& indices) {
    if (indices.NumElements() == 0) {
      return errors::InvalidArgument("Indices are empty");
    }

    auto findices = indices.flat<int>();

    for (std::size_t i = 0; i < findices.dimension(0) - 1; ++i) {
      if (findices(i) < findices(i + 1)) {
        continue;
      }

      return errors::InvalidArgument("Indices are not strictly ordered");
    }

    return Status::OK();
  }