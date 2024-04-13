TEST(EqOp, MatchesScalar) {
    BSONObj operand = BSON("a" << 5);
    EqualityMatchExpression eq("a", operand["a"]);
    ASSERT(eq.matchesBSON(BSON("a" << 5.0), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << 4), NULL));
}