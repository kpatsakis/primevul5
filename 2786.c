string SummarizeArgs(const protobuf::RepeatedPtrField<OpDef::ArgDef>& args) {
  string ret;
  for (const OpDef::ArgDef& arg : args) {
    if (!ret.empty()) strings::StrAppend(&ret, ", ");
    strings::StrAppend(&ret, arg.name(), ":");
    if (arg.is_ref()) strings::StrAppend(&ret, "Ref(");
    if (!arg.number_attr().empty()) {
      strings::StrAppend(&ret, arg.number_attr(), "*");
    }
    if (arg.type() != DT_INVALID) {
      strings::StrAppend(&ret, DataTypeString(arg.type()));
    } else {
      strings::StrAppend(&ret, arg.type_attr());
    }
    if (arg.is_ref()) strings::StrAppend(&ret, ")");
  }
  return ret;
}