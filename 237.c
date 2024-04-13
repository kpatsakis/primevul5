  void DidGetAvailableSpace(QuotaStatusCode status, int64 available_space) {
    quota_status_ = status;
    available_space_ = available_space;
  }
