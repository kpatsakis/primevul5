const ApiDef::Arg* FindInputArg(StringPiece name, const ApiDef& api_def) {
  for (int i = 0; i < api_def.in_arg_size(); ++i) {
    if (api_def.in_arg(i).name() == name) {
      return &api_def.in_arg(i);
    }
  }
  return nullptr;
}