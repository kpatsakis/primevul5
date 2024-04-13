OpDef::AttrDef* FindAttrMutable(StringPiece name, OpDef* op_def) {
  for (int i = 0; i < op_def->attr_size(); ++i) {
    if (op_def->attr(i).name() == name) {
      return op_def->mutable_attr(i);
    }
  }
  return nullptr;
}