void UsageTracker::GetHostUsage(
    const std::string& host, HostUsageCallback* callback) {
  if (client_tracker_map_.size() == 0) {
    callback->Run(host, type_, 0);
    delete callback;
    return;
  }
  if (host_usage_callbacks_.Add(host, callback)) {
    DCHECK(outstanding_host_usage_.find(host) == outstanding_host_usage_.end());
    outstanding_host_usage_[host].pending_clients = client_tracker_map_.size();
    for (ClientTrackerMap::iterator iter = client_tracker_map_.begin();
         iter != client_tracker_map_.end();
         ++iter) {
      iter->second->GetHostUsage(host, callback_factory_.NewCallback(
          &UsageTracker::DidGetClientHostUsage));
    }
  }
}
