      TF_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    if (tuple[index].has_value()) {
      return errors::InvalidArgument("The tensor for index '", index,
                                     "' for key '", key.scalar<int64_t>()(),
                                     "' was already initialized '",
                                     dtypes_.size(), "'.");
    }

    return Status::OK();
  }