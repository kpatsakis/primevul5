TEST(RegexMatchExpression, MatchesElementExtendedOff) {
    BSONObj match = BSON("x"
                         << "a b");
    BSONObj notMatch = BSON("x"
                            << "ab");
    RegexMatchExpression regex("", "a b", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}