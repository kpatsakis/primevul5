TEST(RegexMatchExpression, MatchesElementDotAllOff) {
    BSONObj match = BSON("x"
                         << "a b");
    BSONObj notMatch = BSON("x"
                            << "a\nb");
    RegexMatchExpression regex("", "a.b", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}