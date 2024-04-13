  Status popitem(KeyType* key, const Tensor* indices, Tuple* tuple) {
    tensorflow::mutex_lock lock(mu_);

    // Sanity check the indices
    TF_RETURN_IF_ERROR(check_index_ordering(*indices));

    // Wait until map is not empty
    while (this->map_.empty()) {
      not_empty_.wait(lock);
    }

    // Move from the first element and erase it

    auto it = map_.begin();

    TF_RETURN_IF_ERROR(
        copy_or_move_tensors(&it->second, *key, *indices, tuple));

    *key = it->first;

    // Remove entry if all the values have been consumed
    if (!std::any_of(
            it->second.begin(), it->second.end(),
            [](const OptionalTensor& tensor) { return tensor.has_value(); })) {
      map_.erase(it);
    }

    // Update bytes in the Staging Area
    current_bytes_ -= get_tuple_bytes(*tuple);

    notify_inserters_if_bounded();

    return Status::OK();
  }