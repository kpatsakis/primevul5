  void DidGetModifiedOrigins(const std::set<GURL>& origins, StorageType type) {
    modified_origins_ = origins;
    modified_origins_type_ = type;
  }
