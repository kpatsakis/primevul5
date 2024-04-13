  void DidGetUsageAndQuotaAdditional(
      QuotaStatusCode status, int64 usage, int64 quota) {
    ++additional_callback_count_;
  }
