TEST(RegexMatchExpression, MatchesElementWrongType) {
    BSONObj notMatchInt = BSON("x" << 1);
    BSONObj notMatchBool = BSON("x" << true);
    RegexMatchExpression regex("", "1", "");
    ASSERT(!regex.matchesSingleElement(notMatchInt.firstElement()));
    ASSERT(!regex.matchesSingleElement(notMatchBool.firstElement()));
}