void AddComma(string* s, bool* add_comma) {
  if (*add_comma) {
    strings::StrAppend(s, ", ");
  } else {
    *add_comma = true;
  }
}