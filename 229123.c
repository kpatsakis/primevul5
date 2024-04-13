TEST(RegexMatchExpression, MatchesElementSimplePrefix) {
    BSONObj match = BSON("x"
                         << "abc");
    BSONObj notMatch = BSON("x"
                            << "adz");
    RegexMatchExpression regex("", "^ab", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}