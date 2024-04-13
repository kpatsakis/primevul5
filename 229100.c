TEST(LtOp, MatchesNull) {
    BSONObj operand = BSON("$lt" << BSONNULL);
    LTMatchExpression lt("a", operand["$lt"]);
    ASSERT(!lt.matchesBSON(BSONObj(), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << 4), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(!lt.matchesBSON(BSON("b" << 4), NULL));
}