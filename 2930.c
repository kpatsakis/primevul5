string NodeMissingErrorMsg(absl::string_view node_name) {
  return absl::Substitute("node '$0' was not found", node_name);
}