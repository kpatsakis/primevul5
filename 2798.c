string AllowedStr(const OpDef::AttrDef& attr) {
  if (!attr.has_allowed_values()) return "no restriction";
  return SummarizeAttrValue(attr.allowed_values());
}