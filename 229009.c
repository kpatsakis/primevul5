TEST(RegexMatchExpression, MatchesNull) {
    RegexMatchExpression regex("a", "b", "");
    ASSERT(!regex.matchesBSON(BSONObj(), NULL));
    ASSERT(!regex.matchesBSON(BSON("a" << BSONNULL), NULL));
}