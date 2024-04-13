  void DidGetGlobalUsage(StorageType type, int64 usage,
                         int64 unlimited_usage) {
    type_ = type;
    usage_ = usage;
    unlimited_usage_ = unlimited_usage;
  }
