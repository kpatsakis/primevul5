Status OpDefAttrDefaultsUnchanged(const OpDef& old_op, const OpDef& new_op) {
  AttrMap new_attrs, old_attrs;
  FillAttrMap(old_op, &old_attrs);
  FillAttrMap(new_op, &new_attrs);

  for (const auto& old_attr : old_op.attr()) {
    const OpDef::AttrDef* new_attr =
        gtl::FindPtrOrNull(new_attrs, old_attr.name());
    if (new_attr == nullptr) continue;
    if (new_attr->has_default_value() && !old_attr.has_default_value()) {
      continue;  // Adding new default values is safe.
    }
    if (old_attr.has_default_value() && !new_attr->has_default_value()) {
      return errors::InvalidArgument(
          "Attr '", old_attr.name(), "' has removed it's default; ", "from ",
          DefaultAttrStr(old_attr), " to ", DefaultAttrStr(*new_attr));
    }
    if (old_attr.has_default_value() &&
        !AreAttrValuesEqual(old_attr.default_value(),
                            new_attr->default_value())) {
      return errors::InvalidArgument(
          "Attr '", old_attr.name(), "' has changed it's default value; ",
          "from ", DefaultAttrStr(old_attr), " to ", DefaultAttrStr(*new_attr));
    }
  }

  return Status::OK();
}