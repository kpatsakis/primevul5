  bool operator()(const Tensor& lhs, const Tensor& rhs) const {
    return std::less<int64_t>{}(lhs.scalar<int64_t>()(),
                                rhs.scalar<int64_t>()());
  }