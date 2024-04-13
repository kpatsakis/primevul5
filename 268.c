  void StatusCallback(QuotaStatusCode status) {
    ++status_callback_count_;
    quota_status_ = status;
  }
