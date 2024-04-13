TEST(EqOp, MatchesFullArray) {
    BSONObj operand = BSON("a" << BSON_ARRAY(1 << 2));
    EqualityMatchExpression eq("a", operand["a"]);
    ASSERT(eq.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2 << 3)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(1)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << 1), NULL));
}