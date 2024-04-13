  void DidGetUsageAndQuotaForEviction(QuotaStatusCode status,
      int64 usage, int64 unlimited_usage, int64 quota, int64 available_space) {
    quota_status_ = status;
    usage_ = usage;
    unlimited_usage_ = unlimited_usage;
    quota_ = quota;
    available_space_ = available_space;
  }
