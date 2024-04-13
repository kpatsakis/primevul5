TEST(RegexMatchExpression, MatchesElementDotAllOn) {
    BSONObj match = BSON("x"
                         << "a b");
    BSONObj matchDotAll = BSON("x"
                               << "a\nb");
    BSONObj notMatch = BSON("x"
                            << "ab");
    RegexMatchExpression regex("", "a.b", "s");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(regex.matchesSingleElement(matchDotAll.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}