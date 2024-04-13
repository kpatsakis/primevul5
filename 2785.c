uint64 AttrDefHash(const OpDef::AttrDef& a) {
  uint64 h = Hash64(a.name());
  h = Hash64(a.type().data(), a.type().size(), h);
  h = Hash64Combine(AttrValueHash(a.default_value()), h);
  h = Hash64(a.description().data(), a.description().size(), h);
  h = Hash64Combine(static_cast<uint64>(a.has_minimum()), h);
  h = Hash64Combine(static_cast<uint64>(a.minimum()), h);
  h = Hash64Combine(AttrValueHash(a.allowed_values()), h);
  return h;
}