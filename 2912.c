Status CheckPortRange(int port, int min, int max, ErrorHandler handler) {
  if (port < min || port > max) {
    if (max < min) {
      return handler("no available ports as node has no regular fanins");
    }
    return handler(
        absl::Substitute("port must be in range [$0, $1]", min, max));
  }
  return Status::OK();
}