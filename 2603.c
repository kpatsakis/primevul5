Status ValidateEinsumEllipsis(absl::string_view subscript,
                              bool* found_ellipsis) {
  const int num_periods = absl::c_count(subscript, '.');
  if (num_periods != 0 && num_periods != 3) {
    return errors::InvalidArgument(
        "Expected at most one ellipsis (...), but found ", num_periods,
        " periods (.) in the input subscript: ", subscript);
  }
  if (num_periods == 3 && !absl::StrContains(subscript, "...")) {
    return errors::InvalidArgument(
        "Periods found outside of ellipsis in subscript: ", subscript);
  }
  *found_ellipsis = num_periods > 0;
  return Status::OK();
}