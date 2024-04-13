QuotaManager* QuotaManagerProxy::quota_manager() const {
  DCHECK(!io_thread_ || io_thread_->BelongsToCurrentThread());
  return manager_;
}
