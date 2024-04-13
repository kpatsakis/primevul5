Status ValidateOpDef(const OpDef& op_def) {
  if (!absl::StartsWith(op_def.name(), "_")) {
    VALIDATE(IsValidOpName(op_def.name()), "Invalid name: ", op_def.name(),
             " (Did you use CamelCase?)");
  }

  std::set<string> names;  // for detecting duplicate names
  for (const auto& attr : op_def.attr()) {
    // Validate name
    VALIDATE(gtl::InsertIfNotPresent(&names, attr.name()),
             "Duplicate name: ", attr.name());
    DataType dt;
    VALIDATE(!DataTypeFromString(attr.name(), &dt), "Attr can't have name ",
             attr.name(), " that matches a data type");

    // Validate type
    StringPiece type(attr.type());
    bool is_list = absl::ConsumePrefix(&type, "list(");
    bool found = false;
    for (StringPiece valid : {"string", "int", "float", "bool", "type", "shape",
                              "tensor", "func"}) {
      if (absl::ConsumePrefix(&type, valid)) {
        found = true;
        break;
      }
    }
    VALIDATE(found, "Unrecognized type '", type, "' in attr '", attr.name(),
             "'");
    if (is_list) {
      VALIDATE(absl::ConsumePrefix(&type, ")"),
               "'list(' is missing ')' in attr ", attr.name(), "'s type ",
               attr.type());
    }
    VALIDATE(type.empty(), "Extra '", type, "' at the end of attr ",
             attr.name(), "'s type ", attr.type());

    // Validate minimum
    if (attr.has_minimum()) {
      VALIDATE(attr.type() == "int" || is_list, "Attr '", attr.name(),
               "' has minimum for unsupported type ", attr.type());
      if (is_list) {
        VALIDATE(attr.minimum() >= 0, "Attr '", attr.name(),
                 "' with list type must have a non-negative minimum, not ",
                 attr.minimum());
      }
    } else {
      VALIDATE(attr.minimum() == 0, "Attr '", attr.name(),
               "' with has_minimum = false but minimum ", attr.minimum(),
               " not equal to default of 0");
    }

    // Validate allowed_values
    if (attr.has_allowed_values()) {
      const string list_type =
          is_list ? attr.type() : strings::StrCat("list(", attr.type(), ")");
      TF_RETURN_WITH_CONTEXT_IF_ERROR(
          AttrValueHasType(attr.allowed_values(), list_type), " for attr '",
          attr.name(), "' in Op '", op_def.name(), "'");
    }

    // Validate default_value (after we have validated the rest of the attr,
    // so we can use ValidateAttrValue()).
    if (attr.has_default_value()) {
      TF_RETURN_WITH_CONTEXT_IF_ERROR(
          ValidateAttrValue(attr.default_value(), attr), " in Op '",
          op_def.name(), "'");
    }
  }

  for (const auto& arg : op_def.input_arg()) {
    TF_RETURN_IF_ERROR(ValidateArg(arg, op_def, false, &names));
  }

  for (const auto& arg : op_def.output_arg()) {
    TF_RETURN_IF_ERROR(ValidateArg(arg, op_def, true, &names));
  }

  return Status::OK();
}