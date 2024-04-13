TEST(EqOp, MatchesNull) {
    BSONObj operand = BSON("a" << BSONNULL);
    EqualityMatchExpression eq("a", operand["a"]);
    ASSERT(eq.matchesBSON(BSONObj(), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << 4), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(eq.matchesBSON(BSON("b" << 4), NULL));
}