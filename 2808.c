void AddName(string* s, bool name, const OpDef::ArgDef& arg) {
  if (name) {
    strings::StrAppend(s, arg.name(), ":");
  }
}