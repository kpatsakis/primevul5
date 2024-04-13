  std::size_t size() {
    tensorflow::mutex_lock lock(mu_);
    return map_.size();
  }