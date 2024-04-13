TEST(RegexMatchExpression, MatchesElementUtf8) {
    BSONObj multiByteCharacter = BSON("x"
                                      << "\xc2\xa5");
    RegexMatchExpression regex("", "^.$", "");
    ASSERT(regex.matchesSingleElement(multiByteCharacter.firstElement()));
}