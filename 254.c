  void GetOriginsModifiedSince(StorageType type, base::Time modified_since) {
    modified_origins_.clear();
    modified_origins_type_ = kStorageTypeUnknown;
    quota_manager_->GetOriginsModifiedSince(type, modified_since,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetModifiedOrigins));
  }
