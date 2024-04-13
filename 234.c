  void DeleteOriginFromDatabase(const GURL& origin, StorageType type) {
    quota_manager_->DeleteOriginFromDatabase(origin, type);
  }
