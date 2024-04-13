  void EvictOriginData(const GURL& origin,
                       StorageType type) {
    quota_status_ = kQuotaStatusUnknown;
    quota_manager_->EvictOriginData(origin, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::StatusCallback));
  }
