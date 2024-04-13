TEST(ModMatchExpression, MatchesNull) {
    ModMatchExpression mod("a", 5, 2);
    ASSERT(!mod.matchesBSON(BSONObj(), NULL));
    ASSERT(!mod.matchesBSON(BSON("a" << BSONNULL), NULL));
}