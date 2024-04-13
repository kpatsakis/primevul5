  void NotifyStorageAccessed(QuotaClient* client,
                             const GURL& origin,
                             StorageType type) {
    DCHECK(client);
    quota_manager_->NotifyStorageAccessedInternal(
        client->id(), origin, type, IncrementMockTime());
  }
