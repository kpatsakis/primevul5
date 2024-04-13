  void GetHostUsage(const std::string& host, StorageType type) {
    host_.clear();
    type_ = kStorageTypeUnknown;
    usage_ = -1;
    quota_manager_->GetHostUsage(host, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetHostUsage));
  }
