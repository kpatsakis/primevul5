Status ValidateAttrValue(const AttrValue& attr_value,
                         const OpDef::AttrDef& attr) {
  // Is it a valid value?
  TF_RETURN_WITH_CONTEXT_IF_ERROR(AttrValueHasType(attr_value, attr.type()),
                                  " for attr '", attr.name(), "'");

  // Does the value satisfy the minimum constraint in the AttrDef?
  if (attr.has_minimum()) {
    if (attr.type() == "int") {
      if (attr_value.i() < attr.minimum()) {
        return errors::InvalidArgument(
            "Value for attr '", attr.name(), "' of ", attr_value.i(),
            " must be at least minimum ", attr.minimum());
      }
    } else {
      int length = -1;
      if (attr.type() == "list(string)") {
        length = attr_value.list().s_size();
      } else if (attr.type() == "list(int)") {
        length = attr_value.list().i_size();
      } else if (attr.type() == "list(float)") {
        length = attr_value.list().f_size();
      } else if (attr.type() == "list(bool)") {
        length = attr_value.list().b_size();
      } else if (attr.type() == "list(type)") {
        length = attr_value.list().type_size();
      } else if (attr.type() == "list(shape)") {
        length = attr_value.list().shape_size();
      } else if (attr.type() == "list(tensor)") {
        length = attr_value.list().tensor_size();
      } else if (attr.type() == "list(func)") {
        length = attr_value.list().func_size();
      }
      if (length < attr.minimum()) {
        return errors::InvalidArgument(
            "Length for attr '", attr.name(), "' of ", length,
            " must be at least minimum ", attr.minimum());
      }
    }
  }

  // Does the value satisfy the allowed_value constraint in the AttrDef?
  if (attr.has_allowed_values()) {
    if (attr.type() == "type") {
      TF_RETURN_IF_ERROR(AllowedTypeValue(attr_value.type(), attr));
    } else if (attr.type() == "list(type)") {
      for (int dt : attr_value.list().type()) {
        TF_RETURN_IF_ERROR(AllowedTypeValue(static_cast<DataType>(dt), attr));
      }
    } else if (attr.type() == "string") {
      TF_RETURN_IF_ERROR(AllowedStringValue(attr_value.s(), attr));
    } else if (attr.type() == "list(string)") {
      for (const string& str : attr_value.list().s()) {
        TF_RETURN_IF_ERROR(AllowedStringValue(str, attr));
      }
    } else {
      return errors::Unimplemented(
          "Support for allowed_values not implemented for type ", attr.type());
    }
  }
  return Status::OK();
}