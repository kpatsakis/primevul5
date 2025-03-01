ErrorHandler UpdateFanoutsError(absl::string_view from_node_name,
                                absl::string_view to_node_name) {
  return [from_node_name, to_node_name](absl::string_view msg) {
    string params = absl::Substitute("from_node_name='$0', to_node_name='$1'",
                                     from_node_name, to_node_name);
    return MutationError("UpdateFanouts", params, msg);
  };
}