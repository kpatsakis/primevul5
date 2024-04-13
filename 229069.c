TEST(GtOp, MatchesScalar) {
    BSONObj operand = BSON("$gt" << 5);
    GTMatchExpression gt("a", operand["$gt"]);
    ASSERT(gt.matchesBSON(BSON("a" << 5.5), NULL));
    ASSERT(!gt.matchesBSON(BSON("a" << 4), NULL));
}