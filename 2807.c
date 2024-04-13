string MinStr(const OpDef::AttrDef& attr) {
  if (!attr.has_minimum()) return "no minimum";
  return strings::StrCat(attr.minimum());
}