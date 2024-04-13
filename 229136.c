TEST(RegexMatchExpression, MatchesElementExact) {
    BSONObj match = BSON("a"
                         << "b");
    BSONObj notMatch = BSON("a"
                            << "c");
    RegexMatchExpression regex("", "b", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}