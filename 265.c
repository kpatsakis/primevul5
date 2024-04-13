  void SetPersistentHostQuota(const std::string& host, int64 new_quota) {
    quota_status_ = kQuotaStatusUnknown;
    host_.clear();
    type_ = kStorageTypeUnknown;
    quota_ = -1;
    quota_manager_->SetPersistentHostQuota(host, new_quota,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetHostQuota));
  }
