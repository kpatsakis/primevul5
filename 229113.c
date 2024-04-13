TEST(RegexMatchExpression, MatchesElementCaseSensitive) {
    BSONObj match = BSON("x"
                         << "abc");
    BSONObj notMatch = BSON("x"
                            << "ABC");
    RegexMatchExpression regex("", "abc", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}