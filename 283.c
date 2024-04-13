ClientUsageTracker::ClientUsageTracker(
    UsageTracker* tracker, QuotaClient* client, StorageType type,
    SpecialStoragePolicy* special_storage_policy)
    : tracker_(tracker),
      client_(client),
      type_(type),
      global_usage_(0),
      global_unlimited_usage_(0),
      global_usage_retrieved_(false),
      global_unlimited_usage_is_valid_(true),
      global_usage_task_(NULL),
      special_storage_policy_(special_storage_policy) {
  DCHECK(tracker_);
  DCHECK(client_);
  if (special_storage_policy_)
    special_storage_policy_->AddObserver(this);
}
