Status CheckRemovingFaninFromSelf(absl::string_view node_name,
                                  const TensorId& fanin, ErrorHandler handler) {
  if (node_name == fanin.node()) {
    return handler(absl::Substitute("can't remove fanin '$0' from self",
                                    fanin.ToString()));
  }
  return Status::OK();
}