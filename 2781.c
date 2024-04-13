static Status ValidateArg(const OpDef::ArgDef& arg, const OpDef& op_def,
                          bool output, std::set<string>* names) {
  const string suffix = strings::StrCat(
      output ? " for output '" : " for input '", arg.name(), "'");
  VALIDATE(gtl::InsertIfNotPresent(names, arg.name()),
           "Duplicate name: ", arg.name());
  VALIDATE(HasAttrStyleType(arg), "Missing type", suffix);

  if (!arg.number_attr().empty()) {
    const OpDef::AttrDef* attr = FindAttr(arg.number_attr(), op_def);
    VALIDATE(attr != nullptr, "No attr with name '", arg.number_attr(), "'",
             suffix);
    VALIDATE(attr->type() == "int", "Attr '", attr->name(), "' used as length",
             suffix, " has type ", attr->type(), " != int");
    VALIDATE(attr->has_minimum(), "Attr '", attr->name(), "' used as length",
             suffix, " must have minimum");
    VALIDATE(attr->minimum() >= 0, "Attr '", attr->name(), "' used as length",
             suffix, " must have minimum >= 0");
    VALIDATE(arg.type_list_attr().empty(),
             "Can't have both number_attr and type_list_attr", suffix);
    VALIDATE((arg.type() != DT_INVALID ? 1 : 0) +
                     (!arg.type_attr().empty() ? 1 : 0) ==
                 1,
             "Exactly one of type, type_attr must be set", suffix);
  } else {
    const int num_type_fields = (arg.type() != DT_INVALID ? 1 : 0) +
                                (!arg.type_attr().empty() ? 1 : 0) +
                                (!arg.type_list_attr().empty() ? 1 : 0);
    VALIDATE(num_type_fields == 1,
             "Exactly one of type, type_attr, type_list_attr must be set",
             suffix);
  }

  if (!arg.type_attr().empty()) {
    const OpDef::AttrDef* attr = FindAttr(arg.type_attr(), op_def);
    VALIDATE(attr != nullptr, "No attr with name '", arg.type_attr(), "'",
             suffix);
    VALIDATE(attr->type() == "type", "Attr '", attr->name(),
             "' used as type_attr", suffix, " has type ", attr->type(),
             " != type");
  } else if (!arg.type_list_attr().empty()) {
    const OpDef::AttrDef* attr = FindAttr(arg.type_list_attr(), op_def);
    VALIDATE(attr != nullptr, "No attr with name '", arg.type_list_attr(), "'",
             suffix);
    VALIDATE(attr->type() == "list(type)", "Attr '", attr->name(),
             "' used as type_list_attr", suffix, " has type ", attr->type(),
             " != list(type)");
  } else {
    // All argument types should be non-reference types at this point.
    // ArgDef.is_ref is set to true for reference arguments.
    VALIDATE(!IsRefType(arg.type()), "Illegal use of ref type '",
             DataTypeString(arg.type()), "'. Use 'Ref(type)' instead", suffix);
  }

  return Status::OK();
}