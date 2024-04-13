string ComputeArgSignature(
    const protobuf::RepeatedPtrField<OpDef::ArgDef>& args,
    const AttrMap& old_attrs, const AttrMap& new_attrs, std::vector<bool>* ref,
    bool names) {
  string s;
  bool add_comma = false;
  for (const OpDef::ArgDef& arg : args) {
    if (!arg.type_list_attr().empty()) {
      const OpDef::AttrDef* old_attr =
          gtl::FindPtrOrNull(old_attrs, arg.type_list_attr());
      if (old_attr) {
        // Both old and new have the list(type) attr, so can use it directly.
        AddComma(&s, &add_comma);
        AddName(&s, names, arg);
        strings::StrAppend(&s, arg.type_list_attr());
        ref->push_back(arg.is_ref());
      } else {
        // Missing the list(type) attr in the old, so use the default
        // value for the attr from new instead.
        const OpDef::AttrDef* new_attr =
            gtl::FindPtrOrNull(new_attrs, arg.type_list_attr());
        const auto& type_list = new_attr->default_value().list().type();
        if (type_list.empty()) continue;
        for (int i = 0; i < type_list.size(); ++i) {
          AddComma(&s, &add_comma);
          AddName(&s, names, arg);
          strings::StrAppend(
              &s, DataTypeString(static_cast<DataType>(type_list.Get(i))));
          ref->push_back(arg.is_ref());
        }
      }
    } else {
      int num = 1;  // How many input/outputs does this represent?
      string type;  // What is the type of this arg?
      AddName(&type, names, arg);
      if (!arg.number_attr().empty()) {
        // N * type case.
        const OpDef::AttrDef* old_attr =
            gtl::FindPtrOrNull(old_attrs, arg.number_attr());
        if (old_attr) {
          // Both old and new have the number attr, so can use it directly.
          strings::StrAppend(&type, arg.number_attr(), " * ");
        } else {
          // Missing the number attr in the old, so use the default
          // value for the attr from new instead.
          const OpDef::AttrDef* new_attr =
              gtl::FindPtrOrNull(new_attrs, arg.number_attr());
          num = new_attr->default_value().i();
        }
      }

      if (arg.type() != DT_INVALID) {
        // int32, float, etc. case
        strings::StrAppend(&type, DataTypeString(arg.type()));
      } else {
        const OpDef::AttrDef* old_attr =
            gtl::FindPtrOrNull(old_attrs, arg.type_attr());
        if (old_attr) {
          // Both old and new have the type attr, so can use it directly.
          strings::StrAppend(&type, arg.type_attr());
        } else {
          // Missing the type attr in the old, so use the default
          // value for the attr from new instead.
          const OpDef::AttrDef* new_attr =
              gtl::FindPtrOrNull(new_attrs, arg.type_attr());
          strings::StrAppend(&type,
                             DataTypeString(new_attr->default_value().type()));
        }
      }

      // Record `num` * `type` in the signature.
      for (int i = 0; i < num; ++i) {
        AddComma(&s, &add_comma);
        strings::StrAppend(&s, type);
        ref->push_back(arg.is_ref());
      }
    }
  }

  return s;
}