  std::size_t get_tuple_bytes(const OptionalTuple& tuple) {
    return std::accumulate(
        tuple.begin(), tuple.end(), static_cast<std::size_t>(0),
        [](const std::size_t& lhs, const OptionalTensor& rhs) {
          return (lhs + rhs.has_value()) ? rhs.value().TotalBytes() : 0;
        });
  }