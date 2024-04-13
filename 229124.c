TEST(ExistsMatchExpression, MatchesArray) {
    ExistsMatchExpression exists("a");
    ASSERT(exists.matchesBSON(BSON("a" << BSON_ARRAY(4 << 5.5)), NULL));
}