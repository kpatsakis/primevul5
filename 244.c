   void DidGetUsageAndQuota(QuotaStatusCode status, int64 usage, int64 quota) {
     quota_status_ = status;
     usage_ = usage;
    quota_ = quota;
  }
