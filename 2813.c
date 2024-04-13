Status OpDefAddedDefaultsUnchanged(const OpDef& old_op,
                                   const OpDef& penultimate_op,
                                   const OpDef& new_op) {
  AttrMap new_attrs, old_attrs;
  FillAttrMap(old_op, &old_attrs);
  FillAttrMap(new_op, &new_attrs);

  for (const auto& penultimate_attr : penultimate_op.attr()) {
    const OpDef::AttrDef* old_attr =
        gtl::FindPtrOrNull(old_attrs, penultimate_attr.name());
    if (old_attr != nullptr) continue;  // attr wasn't added
    const OpDef::AttrDef* new_attr =
        gtl::FindPtrOrNull(new_attrs, penultimate_attr.name());

    // These shouldn't happen if the op passed OpDefCompatible().
    if (new_attr == nullptr) {
      return errors::InvalidArgument("Missing attr '", penultimate_attr.name(),
                                     "' in op: ", SummarizeOpDef(new_op));
    }
    if (!penultimate_attr.has_default_value() ||
        !new_attr->has_default_value()) {
      return errors::InvalidArgument("Missing default for attr '",
                                     penultimate_attr.name(),
                                     "' in op: ", SummarizeOpDef(new_op));
    }

    // Actually test that the attr's default value hasn't changed.
    if (!AreAttrValuesEqual(penultimate_attr.default_value(),
                            new_attr->default_value())) {
      return errors::InvalidArgument(
          "Can't change default value for attr '", penultimate_attr.name(),
          "' from ", SummarizeAttrValue(penultimate_attr.default_value()),
          " in op: ", SummarizeOpDef(new_op));
    }
  }

  return Status::OK();
}