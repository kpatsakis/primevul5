TEST(RegexMatchExpression, MatchesElementMultipleFlags) {
    BSONObj matchMultilineDotAll = BSON("x"
                                        << "\na\nb");
    RegexMatchExpression regex("", "^a.b", "ms");
    ASSERT(regex.matchesSingleElement(matchMultilineDotAll.firstElement()));
}