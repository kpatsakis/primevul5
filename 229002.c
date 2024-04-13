RegexMatchExpression::RegexMatchExpression(StringData path, StringData regex, StringData options)
    : LeafMatchExpression(REGEX, path),
      _regex(regex.toString()),
      _flags(options.toString()),
      _re(new pcrecpp::RE(_regex.c_str(), flags2options(_flags.c_str()))) {
    _init();
}