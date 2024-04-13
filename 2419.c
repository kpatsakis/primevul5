  std::size_t get_tuple_bytes(const Tuple& tuple) {
    return std::accumulate(tuple.begin(), tuple.end(),
                           static_cast<std::size_t>(0),
                           [](const std::size_t& lhs, const Tensor& rhs) {
                             return lhs + rhs.TotalBytes();
                           });
  }