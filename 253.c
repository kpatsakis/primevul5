  void GetLRUOrigin(StorageType type) {
    lru_origin_ = GURL();
    quota_manager_->GetLRUOrigin(type,
        callback_factory_.NewCallback(&QuotaManagerTest::DidGetLRUOrigin));
  }
