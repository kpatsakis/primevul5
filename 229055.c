TEST(GteOp, MatchesArrayValue) {
    BSONObj operand = BSON("$gte" << 5);
    GTEMatchExpression gte("a", operand["$gte"]);
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(4 << 5.5)), NULL));
    ASSERT(!gte.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2)), NULL));
}