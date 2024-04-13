 void ClientUsageTracker::GetCachedOrigins(std::set<GURL>* origins) const {
   DCHECK(origins);
   for (HostUsageMap::const_iterator host_iter = cached_usage_.begin();
       host_iter != cached_usage_.end(); host_iter++) {
    const UsageMap& origin_map = host_iter->second;
    for (UsageMap::const_iterator origin_iter = origin_map.begin();
         origin_iter != origin_map.end(); origin_iter++) {
      origins->insert(origin_iter->first);
    }
  }
}
