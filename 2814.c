string SummarizeOpDef(const OpDef& op_def) {
  string ret = strings::StrCat("Op<name=", op_def.name());
  strings::StrAppend(&ret, "; signature=", SummarizeArgs(op_def.input_arg()),
                     " -> ", SummarizeArgs(op_def.output_arg()));
  for (int i = 0; i < op_def.attr_size(); ++i) {
    strings::StrAppend(&ret, "; attr=", op_def.attr(i).name(), ":",
                       op_def.attr(i).type());
    if (op_def.attr(i).has_default_value()) {
      strings::StrAppend(&ret, ",default=",
                         SummarizeAttrValue(op_def.attr(i).default_value()));
    }
    if (op_def.attr(i).has_minimum()) {
      strings::StrAppend(&ret, ",min=", op_def.attr(i).minimum());
    }
    if (op_def.attr(i).has_allowed_values()) {
      strings::StrAppend(&ret, ",allowed=",
                         SummarizeAttrValue(op_def.attr(i).allowed_values()));
    }
  }
  if (op_def.is_commutative()) {
    strings::StrAppend(&ret, "; is_commutative=true");
  }
  if (op_def.is_aggregate()) {
    strings::StrAppend(&ret, "; is_aggregate=true");
  }
  if (op_def.is_stateful()) {
    strings::StrAppend(&ret, "; is_stateful=true");
  }
  if (op_def.allows_uninitialized_input()) {
    strings::StrAppend(&ret, "; allows_uninitialized_input=true");
  }
  if (op_def.is_distributed_communication()) {
    strings::StrAppend(&ret, "; is_distributed_communication=true");
  }
  strings::StrAppend(&ret, ">");
  return ret;
}