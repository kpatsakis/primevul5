  void DeleteOriginData(const GURL& origin,
                        StorageType type) {
    quota_status_ = kQuotaStatusUnknown;
    quota_manager_->DeleteOriginData(origin, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::StatusCallback));
  }
