bool MoreRestrictive(const OpDef::AttrDef& old_attr,
                     const OpDef::AttrDef& new_attr) {
  // Anything -> no restriction : not more restrictive.
  if (!new_attr.has_allowed_values()) return false;
  // No restriction -> restriction : more restrictive.
  if (!old_attr.has_allowed_values()) return true;
  // If anything that was previously allowed is no longer allowed:
  // more restrictive.
  if (!IsSubsetOf(old_attr.allowed_values().list().type(),
                  new_attr.allowed_values().list().type())) {
    return true;
  }
  if (!IsSubsetOf(old_attr.allowed_values().list().s(),
                  new_attr.allowed_values().list().s())) {
    return true;
  }
  return false;
}