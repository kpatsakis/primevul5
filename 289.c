  GatherHostUsageTask(
      UsageTracker* tracker,
      QuotaClient* client,
      const std::string& host)
      : GatherUsageTaskBase(tracker, client),
        client_(client),
        host_(host),
        callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
    DCHECK(client_);
  }
