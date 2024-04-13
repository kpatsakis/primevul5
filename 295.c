void ClientUsageTracker::GetGlobalUsage(GlobalUsageCallback* callback) {
  if (global_usage_retrieved_) {
    callback->Run(type_, global_usage_, GetCachedGlobalUnlimitedUsage());
    delete callback;
    return;
  }
  DCHECK(!global_usage_callback_.HasCallbacks());
  global_usage_callback_.Add(callback);
  global_usage_task_ = new GatherGlobalUsageTask(tracker_, client_);
  global_usage_task_->Start();
}
