const OpDef::ArgDef* FindInputArg(StringPiece name, const OpDef& op_def) {
  for (int i = 0; i < op_def.input_arg_size(); ++i) {
    if (op_def.input_arg(i).name() == name) {
      return &op_def.input_arg(i);
    }
  }
  return nullptr;
}