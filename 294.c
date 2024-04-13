void UsageTracker::GetGlobalUsage(GlobalUsageCallback* callback) {
  if (client_tracker_map_.size() == 0) {
    callback->Run(type_, 0, 0);
    delete callback;
    return;
  }
  if (global_usage_callbacks_.Add(callback)) {
    global_usage_.pending_clients = client_tracker_map_.size();
    global_usage_.usage = 0;
    global_usage_.unlimited_usage = 0;
    for (ClientTrackerMap::iterator iter = client_tracker_map_.begin();
         iter != client_tracker_map_.end();
         ++iter) {
      iter->second->GetGlobalUsage(callback_factory_.NewCallback(
          &UsageTracker::DidGetClientGlobalUsage));
    }
  }
}
