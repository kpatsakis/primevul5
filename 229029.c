TEST(LtOp, MatchesArrayValue) {
    BSONObj operand = BSON("$lt" << 5);
    LTMatchExpression lt("a", operand["$lt"]);
    ASSERT(lt.matchesBSON(BSON("a" << BSON_ARRAY(6 << 4.5)), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSON_ARRAY(6 << 7)), NULL));
}