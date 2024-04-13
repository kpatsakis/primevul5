  void SetTemporaryGlobalQuota(int64 new_quota) {
    quota_status_ = kQuotaStatusUnknown;
    quota_ = -1;
    quota_manager_->SetTemporaryGlobalQuota(new_quota,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetQuota));
  }
