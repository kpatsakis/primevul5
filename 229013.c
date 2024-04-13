void RegexMatchExpression::_init() {
    uassert(
        ErrorCodes::BadValue, "Regular expression is too long", _regex.size() <= kMaxPatternSize);

    uassert(ErrorCodes::BadValue,
            "Regular expression cannot contain an embedded null byte",
            _regex.find('\0') == std::string::npos);

    uassert(ErrorCodes::BadValue,
            "Regular expression options string cannot contain an embedded null byte",
            _flags.find('\0') == std::string::npos);

    // isValidUTF8() checks for UTF-8 which does not map to a series of codepoints but does not
    // check the validity of the code points themselves. These situations do not cause problems
    // downstream so we do not do additional work to enforce that the code points are valid.
    uassert(
        5108300, "Regular expression is invalid UTF-8", isValidUTF8(_regex) && isValidUTF8(_flags));
}