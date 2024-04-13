 void UsageTracker::GetCachedOrigins(std::set<GURL>* origins) const {
   DCHECK(origins);
   origins->clear();
  for (ClientTrackerMap::const_iterator iter = client_tracker_map_.begin();
       iter != client_tracker_map_.end(); ++iter) {
    iter->second->GetCachedOrigins(origins);
  }
}
