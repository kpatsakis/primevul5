      TF_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    auto findices = indices.flat<int>();

    // Return values at specified indices
    for (std::size_t i = 0; i < findices.dimension(0); ++i) {
      std::size_t index = findices(i);

      TF_RETURN_IF_ERROR(check_index(key, index));

      // Insist on a value present at the specified index
      if (!(*map_tuple)[index].has_value()) {
        return Status(errors::InvalidArgument(
            "Tensor at index '", index, "' for key '", key.scalar<int64_t>()(),
            "' has already been removed."));
      }

      // Copy the contained tensor and
      // remove from the OptionalTuple
      output->push_back((*map_tuple)[index].value());

      // Clear out the entry if we're not copying (moving)
      if (!copy) {
        (*map_tuple)[index].reset();
      }
    }

    return Status::OK();
  }