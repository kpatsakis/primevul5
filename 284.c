void UsageTracker::DidGetClientGlobalUsage(StorageType type,
                                           int64 usage,
                                           int64 unlimited_usage) {
  DCHECK_EQ(type, type_);
  global_usage_.usage += usage;
  global_usage_.unlimited_usage += unlimited_usage;
  if (--global_usage_.pending_clients == 0) {
    if (global_usage_.usage < 0)
      global_usage_.usage = 0;
    if (global_usage_.unlimited_usage > global_usage_.usage)
      global_usage_.unlimited_usage = global_usage_.usage;
    else if (global_usage_.unlimited_usage < 0)
      global_usage_.unlimited_usage = 0;

    global_usage_callbacks_.Run(type, global_usage_.usage,
                                global_usage_.unlimited_usage);
  }
}
