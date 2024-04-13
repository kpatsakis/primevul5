Status AllowedStringValue(const string& str, const OpDef::AttrDef& attr) {
  const AttrValue& allowed_values(attr.allowed_values());
  for (const auto& allowed : allowed_values.list().s()) {
    if (str == allowed) {
      return Status::OK();
    }
  }
  string allowed_str;
  for (const string& allowed : allowed_values.list().s()) {
    if (!allowed_str.empty()) {
      strings::StrAppend(&allowed_str, ", ");
    }
    strings::StrAppend(&allowed_str, "\"", allowed, "\"");
  }
  return errors::InvalidArgument(
      "Value for attr '", attr.name(), "' of \"", str,
      "\" is not in the list of allowed values: ", allowed_str);
}