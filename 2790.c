uint64 OpDefHash(const OpDef& o) {
  uint64 h = RepeatedAttrDefHash(o.attr());

  // Compute deterministic order-independent control outputs hash.
  std::set<string> control_output(o.control_output().begin(),
                                  o.control_output().end());
  for (const auto& co : control_output) h = Hash64Combine(h, Hash64(co));

  OpDef o_copy = o;
  o_copy.clear_attr();
  o_copy.clear_control_output();
  return DeterministicProtoHash64(o_copy, h);
}