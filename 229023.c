TEST(GtOp, MatchesArrayValue) {
    BSONObj operand = BSON("$gt" << 5);
    GTMatchExpression gt("a", operand["$gt"]);
    ASSERT(gt.matchesBSON(BSON("a" << BSON_ARRAY(3 << 5.5)), NULL));
    ASSERT(!gt.matchesBSON(BSON("a" << BSON_ARRAY(2 << 4)), NULL));
}