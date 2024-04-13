Status OpDefCompatible(const OpDef& old_op, const OpDef& new_op) {
#define VALIDATE(CONDITION, ...)                                            \
  if (!(CONDITION)) {                                                       \
    return errors::InvalidArgument("Incompatible Op change: ", __VA_ARGS__, \
                                   "; old: ", SummarizeOpDef(old_op),       \
                                   "; new: ", SummarizeOpDef(new_op));      \
  }

  VALIDATE(old_op.name() == new_op.name(), "Name mismatch");

  AttrMap new_attrs, old_attrs;
  FillAttrMap(old_op, &old_attrs);
  FillAttrMap(new_op, &new_attrs);
  for (const auto& old_attr : old_op.attr()) {
    const OpDef::AttrDef* new_attr =
        gtl::FindPtrOrNull(new_attrs, old_attr.name());
    VALIDATE(new_attr != nullptr, "Attr '", old_attr.name(), "' removed");
    VALIDATE(old_attr.type() == new_attr->type(), "Attr '", old_attr.name(),
             "' changed type '", old_attr.type(), "' -> '", new_attr->type(),
             "'");
    VALIDATE(!MoreRestrictive(old_attr, *new_attr), "Attr '", old_attr.name(),
             "' has a stricter set of allowed values; from ",
             AllowedStr(old_attr), " to ", AllowedStr(*new_attr));
    VALIDATE(!HigherMinimum(old_attr, *new_attr), "Attr '", old_attr.name(),
             "' has a higher minimum; from ", MinStr(old_attr), " to ",
             MinStr(*new_attr));
  }

  for (const auto& new_attr : new_op.attr()) {
    const OpDef::AttrDef* old_attr =
        gtl::FindPtrOrNull(old_attrs, new_attr.name());
    VALIDATE(old_attr != nullptr || new_attr.has_default_value(), "Attr '",
             new_attr.name(), "' added without default");
  }

  std::vector<bool> old_in_ref, new_in_ref, old_out_ref, new_out_ref;
  const string old_in_sig = ComputeArgSignature(
      old_op.input_arg(), old_attrs, new_attrs, &old_in_ref, false /* names */);
  const string new_in_sig = ComputeArgSignature(
      new_op.input_arg(), old_attrs, new_attrs, &new_in_ref, false /* names */);
  VALIDATE(old_in_sig == new_in_sig, "Input signature mismatch '", old_in_sig,
           "' vs. '", new_in_sig, "'");
  VALIDATE(old_in_ref.size() == new_in_ref.size(),  // Should not happen
           "Unexpected change in input ref lists.");
  for (int i = 0, end = old_in_ref.size(); i < end; ++i) {
    // Allowed to remove "ref" from an input (or leave it unchanged).
    VALIDATE(old_in_ref[i] || !new_in_ref[i], "Input ", i,
             " changed from non-ref to ref");
  }

  const string old_out_sig =
      ComputeArgSignature(old_op.output_arg(), old_attrs, new_attrs,
                          &old_out_ref, true /* names */);
  const string new_out_sig =
      ComputeArgSignature(new_op.output_arg(), old_attrs, new_attrs,
                          &new_out_ref, true /* names */);
  VALIDATE(old_out_sig == new_out_sig, "Output signature mismatch '",
           old_out_sig, "' vs. '", new_out_sig, "'");
  VALIDATE(old_out_ref.size() == new_out_ref.size(),  // Should not happen
           "Unexpected change in output ref lists");
  for (int i = 0, end = old_out_ref.size(); i < end; ++i) {
    // Allowed to add "ref" to an output (or leave it unchanged).
    VALIDATE(!old_out_ref[i] || new_out_ref[i], "Output ", i,
             " changed from ref to non-ref");
  }

  return Status::OK();
}