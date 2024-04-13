TEST(LtOp, MatchesScalar) {
    BSONObj operand = BSON("$lt" << 5);
    LTMatchExpression lt("a", operand["$lt"]);
    ASSERT(lt.matchesBSON(BSON("a" << 4.5), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << 6), NULL));
}