  Status clear() {
    tensorflow::mutex_lock lock(mu_);
    map_.clear();
    incomplete_.clear();
    current_bytes_ = 0;

    notify_inserters_if_bounded();

    return Status::OK();
  }