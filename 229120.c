TEST(ModMatchExpression, MatchesScalar) {
    ModMatchExpression mod("a", 5, 2);
    ASSERT(mod.matchesBSON(BSON("a" << 7.0), NULL));
    ASSERT(!mod.matchesBSON(BSON("a" << 4), NULL));
}