   void GetUsageAndQuota(const GURL& origin, StorageType type) {
     quota_status_ = kQuotaStatusUnknown;
     usage_ = -1;
    quota_ = -1;
    quota_manager_->GetUsageAndQuota(origin, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetUsageAndQuota));
  }
