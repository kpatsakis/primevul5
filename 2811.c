string DefaultAttrStr(const OpDef::AttrDef& attr) {
  if (!attr.has_default_value()) return "no default";
  return SummarizeAttrValue(attr.default_value());
}