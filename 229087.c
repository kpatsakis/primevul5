TEST(LtOp, MatchesScalarEmptyKey) {
    BSONObj operand = BSON("$lt" << 5);
    LTMatchExpression lt("", operand["$lt"]);
    ASSERT(lt.matchesBSON(BSON("" << 4.5), NULL));
    ASSERT(!lt.matchesBSON(BSON("" << 6), NULL));
}