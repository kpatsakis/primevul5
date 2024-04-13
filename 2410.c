  Status get(const KeyType* key, const Tensor* indices, Tuple* tuple) {
    tensorflow::mutex_lock lock(mu_);

    // Sanity check the indices
    TF_RETURN_IF_ERROR(check_index_ordering(*indices));

    typename MapType::iterator it;

    // Wait until the element with the requested key is present
    while ((it = map_.find(*key)) == map_.end()) {
      not_empty_.wait(lock);
    }

    TF_RETURN_IF_ERROR(
        copy_or_move_tensors(&it->second, *key, *indices, tuple, true));

    // Update bytes in the Staging Area
    current_bytes_ -= get_tuple_bytes(*tuple);

    return Status::OK();
  }