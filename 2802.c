bool AttrDefEqual(const OpDef::AttrDef& a1, const OpDef::AttrDef& a2) {
  if (std::is_base_of<protobuf::Message, OpDef::AttrDef>()) {
    DCHECK_EQ(7, reinterpret_cast<const protobuf::Message*>(&a1)
                     ->GetDescriptor()
                     ->field_count())
        << "Please modify these equality and hash functions to reflect the "
           "changes to the AttrDef protobuf";
  }

  if (a1.name() != a2.name()) return false;
  if (a1.type() != a2.type()) return false;
  if (a1.description() != a2.description()) return false;
  if (a1.has_minimum() != a2.has_minimum()) return false;
  if (a1.has_minimum() && a1.minimum() != a2.minimum()) return false;
  if (!AreAttrValuesEqual(a1.default_value(), a2.default_value())) return false;
  if (!AreAttrValuesEqual(a1.allowed_values(), a2.allowed_values()))
    return false;
  return true;
}