void ClientUsageTracker::GatherHostUsageComplete(const std::string& host) {
  DCHECK(host_usage_tasks_.find(host) != host_usage_tasks_.end());
  host_usage_tasks_.erase(host);
   host_usage_callbacks_.Run(host, host, type_, GetCachedHostUsage(host));
 }
