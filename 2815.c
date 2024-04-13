uint64 RepeatedAttrDefHash(
    const protobuf::RepeatedPtrField<OpDef::AttrDef>& a) {
  // Insert AttrDefs into map to deterministically sort by name
  std::map<string, const OpDef::AttrDef*> a_set;
  for (const OpDef::AttrDef& def : a) {
    a_set[def.name()] = &def;
  }
  // Iterate and combines hashes of keys and values
  uint64 h = 0xDECAFCAFFE;
  for (const auto& pair : a_set) {
    h = Hash64(pair.first.data(), pair.first.size(), h);
    h = Hash64Combine(AttrDefHash(*pair.second), h);
  }
  return h;
}