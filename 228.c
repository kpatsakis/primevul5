QuotaManager::~QuotaManager() {
  DCHECK(io_thread_->BelongsToCurrentThread());
  proxy_->manager_ = NULL;
  std::for_each(clients_.begin(), clients_.end(),
                std::mem_fun(&QuotaClient::OnQuotaManagerDestroyed));
  if (database_.get())
     db_thread_->DeleteSoon(FROM_HERE, database_.release());
 }
