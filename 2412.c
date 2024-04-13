  std::size_t operator()(const Tensor& key) const {
    return std::hash<int64_t>{}(key.scalar<int64_t>()());
  }