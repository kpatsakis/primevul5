void ClientUsageTracker::AddCachedOrigin(
    const GURL& origin, int64 usage) {
  std::string host = net::GetHostOrSpecFromURL(origin);
  UsageMap::iterator iter = cached_usage_[host].
      insert(UsageMap::value_type(origin, 0)).first;
  int64 old_usage = iter->second;
  iter->second = usage;
  int64 delta = usage - old_usage;
  if (delta) {
    global_usage_ += delta;
    if (global_unlimited_usage_is_valid_ && IsStorageUnlimited(origin))
      global_unlimited_usage_ += delta;
  }
  DCHECK_GE(iter->second, 0);
  DCHECK_GE(global_usage_, 0);
}
