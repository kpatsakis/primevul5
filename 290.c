int64 ClientUsageTracker::GetCachedGlobalUnlimitedUsage() {
  if (!global_unlimited_usage_is_valid_) {
    global_unlimited_usage_ = 0;
    for (HostUsageMap::const_iterator host_iter = cached_usage_.begin();
         host_iter != cached_usage_.end(); host_iter++) {
      const UsageMap& origin_map = host_iter->second;
      for (UsageMap::const_iterator origin_iter = origin_map.begin();
           origin_iter != origin_map.end(); origin_iter++) {
        if (IsStorageUnlimited(origin_iter->first))
          global_unlimited_usage_ += origin_iter->second;
      }
    }
    global_unlimited_usage_is_valid_ = true;
  }
  return global_unlimited_usage_;
}
