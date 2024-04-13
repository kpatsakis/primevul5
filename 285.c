void UsageTracker::DidGetClientHostUsage(const std::string& host,
                                         StorageType type,
                                         int64 usage) {
  DCHECK_EQ(type, type_);
  TrackingInfo& info = outstanding_host_usage_[host];
  info.usage += usage;
  if (--info.pending_clients == 0) {
    if (info.usage < 0)
      info.usage = 0;
    host_usage_callbacks_.Run(host, host, type, info.usage);
    outstanding_host_usage_.erase(host);
  }
}
