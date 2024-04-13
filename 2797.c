bool HigherMinimum(const OpDef::AttrDef& old_attr,
                   const OpDef::AttrDef& new_attr) {
  // Anything -> no restriction : not more restrictive.
  if (!new_attr.has_minimum()) return false;
  // No restriction -> restriction : more restrictive.
  if (!old_attr.has_minimum()) return true;
  // If anything that was previously allowed is no longer allowed:
  // more restrictive.
  return new_attr.minimum() > old_attr.minimum();
}