  void GetGlobalUsage(StorageType type) {
    type_ = kStorageTypeUnknown;
    usage_ = -1;
    unlimited_usage_ = -1;
    quota_manager_->GetGlobalUsage(type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetGlobalUsage));
  }
