void RemoveDescriptionsFromOpDef(OpDef* op_def) {
  RemoveNonDeprecationDescriptionsFromOpDef(op_def);
  if (op_def->has_deprecation()) {
    op_def->mutable_deprecation()->clear_explanation();
  }
}