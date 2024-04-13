  virtual bool is_expensive()
  {
    if (value_cached)
      return false;
    return example->is_expensive();
  }