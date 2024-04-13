TEST(RegexMatchExpression, MatchesElementCaseInsensitive) {
    BSONObj match = BSON("x"
                         << "abc");
    BSONObj matchUppercase = BSON("x"
                                  << "ABC");
    BSONObj notMatch = BSON("x"
                            << "abz");
    RegexMatchExpression regex("", "abc", "i");
    ASSERT(regex.matchesSingleElement(match.firstElement()));
    ASSERT(regex.matchesSingleElement(matchUppercase.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatch.firstElement()));
}