TEST(EqOp, MatchesArrayValue) {
    BSONObj operand = BSON("a" << 5);
    EqualityMatchExpression eq("a", operand["a"]);
    ASSERT(eq.matchesBSON(BSON("a" << BSON_ARRAY(5.0 << 6)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(6 << 7)), NULL));
}