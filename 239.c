  void DidGetHostQuota(QuotaStatusCode status,
                       const std::string& host,
                       StorageType type,
                       int64 quota) {
    quota_status_ = status;
    host_ = host;
    type_ = type;
    quota_ = quota;
  }
