bool ClientUsageTracker::IsStorageUnlimited(const GURL& origin) const {
  return special_storage_policy_.get() &&
         special_storage_policy_->IsStorageUnlimited(origin);
}
