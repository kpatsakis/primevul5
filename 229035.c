TEST(GteOp, MatchesScalar) {
    BSONObj operand = BSON("$gte" << 5);
    GTEMatchExpression gte("a", operand["$gte"]);
    ASSERT(gte.matchesBSON(BSON("a" << 5.5), NULL));
    ASSERT(!gte.matchesBSON(BSON("a" << 4), NULL));
}