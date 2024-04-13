      TF_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    if (index >= dtypes_.size()) {
      return Status(errors::InvalidArgument(
          "Index '", index, "' for key '", key.scalar<int64_t>()(),
          "' was out of bounds '", dtypes_.size(), "'."));
    }

    return Status::OK();
  }