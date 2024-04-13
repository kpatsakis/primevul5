Status CheckFaninIsRegular(const TensorId& fanin, ErrorHandler handler) {
  if (!IsTensorIdRegular(fanin)) {
    return handler(absl::Substitute("fanin '$0' must be a regular tensor id",
                                    fanin.ToString()));
  }
  return Status::OK();
}