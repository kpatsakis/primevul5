  std::size_t incomplete_size() {
    tensorflow::mutex_lock lock(mu_);
    return incomplete_.size();
  }