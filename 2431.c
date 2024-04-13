  Status put(KeyType* key, const Tensor* indices, OptionalTuple* tuple) {
    tensorflow::mutex_lock lock(mu_);

    // Sanity check the indices
    TF_RETURN_IF_ERROR(check_index_ordering(*indices));

    // Handle incomplete inserts
    if (indices->NumElements() != dtypes_.size()) {
      return put_incomplete(*key, *indices, tuple, &lock);
    }

    std::size_t tuple_bytes = get_tuple_bytes(*tuple);
    // Check that tuple_bytes fits within the memory limit
    TF_RETURN_IF_ERROR(check_memory_limit(tuple_bytes));

    // Wait until there's space for insertion.
    while (would_exceed_memory_limit(tuple_bytes) || is_capacity_full()) {
      full_.wait(lock);
    }

    // Do the put operation
    TF_RETURN_IF_ERROR(put_complete(*key, tuple));

    // Update the current size
    current_bytes_ += tuple_bytes;

    return Status::OK();
  }