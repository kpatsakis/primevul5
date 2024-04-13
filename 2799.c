bool HasAttrStyleType(const OpDef::ArgDef& arg) {
  return arg.type() != DT_INVALID || !arg.type_attr().empty() ||
         !arg.type_list_attr().empty();
}