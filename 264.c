  void RunAdditionalUsageAndQuotaTask(const GURL& origin, StorageType type) {
    quota_manager_->GetUsageAndQuota(origin, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidGetUsageAndQuotaAdditional));
  }
