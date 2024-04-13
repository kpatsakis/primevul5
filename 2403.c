      TF_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    if (has_memory_limit() && bytes > memory_limit_) {
      return errors::ResourceExhausted(
          "Attempted to insert tensors with combined size of '", bytes,
          "' bytes into Staging Area with a memory limit of '", memory_limit_,
          "'.");
    }

    return Status::OK();
  }