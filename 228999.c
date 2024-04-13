TEST(ExistsMatchExpression, MatchesScalar) {
    ExistsMatchExpression exists("a");
    ASSERT(exists.matchesBSON(BSON("a" << 1), NULL));
    ASSERT(exists.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!exists.matchesBSON(BSON("b" << 1), NULL));
}