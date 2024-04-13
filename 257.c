  void GetUsageAndQuotaForEviction() {
    quota_status_ = kQuotaStatusUnknown;
    usage_ = -1;
    unlimited_usage_ = -1;
    quota_ = -1;
    available_space_ = -1;
    quota_manager_->GetUsageAndQuotaForEviction(
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetUsageAndQuotaForEviction));
  }
