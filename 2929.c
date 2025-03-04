Status CheckAddingFaninToSelf(absl::string_view node_name,
                              const TensorId& fanin, ErrorHandler handler) {
  if (node_name == fanin.node()) {
    return handler(
        absl::Substitute("can't add fanin '$0' to self", fanin.ToString()));
  }
  return Status::OK();
}