Status CheckFaninIsValid(const TensorId& fanin, ErrorHandler handler) {
  if (!IsTensorIdPortValid(fanin)) {
    return handler(absl::Substitute("fanin '$0' must be a valid tensor id",
                                    fanin.ToString()));
  }
  return Status::OK();
}