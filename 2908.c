Status MutationError(absl::string_view function_name, absl::string_view params,
                     absl::string_view msg) {
  return errors::InvalidArgument(absl::Substitute(
      "MutableGraphView::$0($1) error: $2.", function_name, params, msg));
}