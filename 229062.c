TEST(ModMatchExpression, MatchesArrayValue) {
    ModMatchExpression mod("a", 5, 2);
    ASSERT(mod.matchesBSON(BSON("a" << BSON_ARRAY(5 << 12LL)), NULL));
    ASSERT(!mod.matchesBSON(BSON("a" << BSON_ARRAY(6 << 8)), NULL));
}