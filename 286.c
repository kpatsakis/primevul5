  void DidGetUsage(int64 usage) {
    DCHECK(original_message_loop()->BelongsToCurrentThread());
    DCHECK(!pending_origins_.empty());
    DCHECK(client_tracker_);

    DCHECK_GE(usage, 0);
    if (usage < 0)
      usage = 0;

    const GURL& origin = pending_origins_.front();
    std::string host = net::GetHostOrSpecFromURL(origin);
    client_tracker_->AddCachedOrigin(origin, usage);

    pending_origins_.pop_front();
    if (pending_origins_.empty() ||
        host != net::GetHostOrSpecFromURL(pending_origins_.front())) {
      client_tracker_->AddCachedHost(host);
    }

    if (pending_origins_.empty()) {
      CallCompleted();
      DeleteSoon();
    }
  }
