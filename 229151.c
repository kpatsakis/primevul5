RegexMatchExpression::RegexMatchExpression(StringData path, const BSONElement& e)
    : LeafMatchExpression(REGEX, path),
      _regex(e.regex()),
      _flags(e.regexFlags()),
      _re(new pcrecpp::RE(_regex.c_str(), flags2options(_flags.c_str()))) {
    uassert(ErrorCodes::BadValue, "regex not a regex", e.type() == RegEx);
    _init();
}