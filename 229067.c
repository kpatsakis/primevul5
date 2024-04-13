TEST(RegexMatchExpression, MatchesElementMultilineOn) {
    BSONObj match = BSON("x"
                         << "az");
    BSONObj matchMultiline = BSON("x"
                                  << "\naz");
    BSONObj notMatch = BSON("x"
                            << "\n\n");
    RegexMatchExpression regex("", "^a", "m");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(regex.matchesSingleElement(matchMultiline.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}