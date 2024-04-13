TEST(GtOp, MatchesNull) {
    BSONObj operand = BSON("$gt" << BSONNULL);
    GTMatchExpression gt("a", operand["$gt"]);
    ASSERT(!gt.matchesBSON(BSONObj(), NULL));
    ASSERT(!gt.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!gt.matchesBSON(BSON("a" << 4), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(!gt.matchesBSON(BSON("b" << 4), NULL));
}