TEST(LteOp, MatchesArrayValue) {
    BSONObj operand = BSON("$lte" << 5);
    LTEMatchExpression lte("a", operand["$lte"]);
    ASSERT(lte.matchesBSON(BSON("a" << BSON_ARRAY(6 << 4.5)), NULL));
    ASSERT(!lte.matchesBSON(BSON("a" << BSON_ARRAY(6 << 7)), NULL));
}