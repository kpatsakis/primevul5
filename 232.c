  void DeleteClientOriginData(QuotaClient* client,
                        const GURL& origin,
                        StorageType type) {
    DCHECK(client);
    quota_status_ = kQuotaStatusUnknown;
    client->DeleteOriginData(origin, type,
        callback_factory_.NewCallback(
            &QuotaManagerTest::StatusCallback));
  }
