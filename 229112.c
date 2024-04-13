TEST(RegexMatchExpression, MatchesElementMultilineOff) {
    BSONObj match = BSON("x"
                         << "az");
    BSONObj notMatch = BSON("x"
                            << "\naz");
    RegexMatchExpression regex("", "^a", "");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}