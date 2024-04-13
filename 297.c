  void GetUsageForOrigins(const std::set<GURL>& origins, StorageType type) {
    DCHECK(original_message_loop()->BelongsToCurrentThread());
    std::vector<GURL> origins_to_gather;
    std::set<GURL> cached_origins;
    client_tracker()->GetCachedOrigins(&cached_origins);
    std::set<GURL> already_added;
    for (std::set<GURL>::const_iterator iter = origins.begin();
         iter != origins.end(); ++iter) {
      if (cached_origins.find(*iter) == cached_origins.end() &&
          already_added.insert(*iter).second) {
        origins_to_gather.push_back(*iter);
      }
    }
    if (origins_to_gather.empty()) {
      CallCompleted();
      DeleteSoon();
      return;
    }

    std::sort(origins_to_gather.begin(), origins_to_gather.end(), SortByHost);

    for (std::vector<GURL>::const_iterator iter = origins_to_gather.begin();
         iter != origins_to_gather.end(); iter++)
      pending_origins_.push_back(*iter);

    for (std::vector<GURL>::const_iterator iter = origins_to_gather.begin();
         iter != origins_to_gather.end(); iter++)
      client_->GetOriginUsage(
          *iter,
          tracker_->type(),
          callback_factory_.NewCallback(&GatherUsageTaskBase::DidGetUsage));
  }
