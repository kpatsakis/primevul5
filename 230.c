  virtual ~UsageAndQuotaDispatcherTask() {
    STLDeleteContainerPointers(callbacks_.begin(), callbacks_.end());
    STLDeleteContainerPointers(unlimited_callbacks_.begin(),
                               unlimited_callbacks_.end());
  }
