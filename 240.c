  void DidGetHostUsage(const std::string& host,
                       StorageType type,
                       int64 usage) {
    host_ = host;
    type_ = type;
    usage_ = usage;
  }
