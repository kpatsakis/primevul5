void FillAttrMap(const OpDef& op_def, AttrMap* attr_map) {
  for (const auto& attr : op_def.attr()) {
    (*attr_map)[attr.name()] = &attr;
  }
}