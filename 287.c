void ClientUsageTracker::GatherGlobalUsageComplete() {
  DCHECK(global_usage_task_ != NULL);
  global_usage_task_ = NULL;
  global_usage_retrieved_ = true;

  DCHECK(global_usage_callback_.HasCallbacks());
  global_usage_callback_.Run(type_, global_usage_,
                             GetCachedGlobalUnlimitedUsage());

  for (HostUsageCallbackMap::iterator iter = host_usage_callbacks_.Begin();
       iter != host_usage_callbacks_.End(); ++iter) {
    iter->second.Run(iter->first, type_, GetCachedHostUsage(iter->first));
  }
  host_usage_callbacks_.Clear();
}
