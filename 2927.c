string SwapNodeNamesSwitchControlErrorMsg(absl::string_view node_name) {
  return absl::Substitute(
      "can't swap node name '$0' as it will become a Switch control dependency",
      node_name);
}