  void DidGetQuota(QuotaStatusCode status,
                   StorageType type,
                   int64 quota) {
    quota_status_ = status;
    type_ = type;
    quota_ = quota;
  }
