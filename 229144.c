TEST(RegexMatchExpression, MatchesElementExtendedOn) {
    BSONObj match = BSON("x"
                         << "ab");
    BSONObj notMatch = BSON("x"
                            << "a b");
    RegexMatchExpression regex("", "a b", "x");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}