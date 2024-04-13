TEST(LteOp, MatchesScalar) {
    BSONObj operand = BSON("$lte" << 5);
    LTEMatchExpression lte("a", operand["$lte"]);
    ASSERT(lte.matchesBSON(BSON("a" << 4.5), NULL));
    ASSERT(!lte.matchesBSON(BSON("a" << 6), NULL));
}