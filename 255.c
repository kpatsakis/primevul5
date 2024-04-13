  void GetTemporaryGlobalQuota() {
    quota_status_ = kQuotaStatusUnknown;
    quota_ = -1;
    quota_manager_->GetTemporaryGlobalQuota(
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetQuota));
  }
