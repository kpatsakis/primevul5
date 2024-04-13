TEST(GteOp, MatchesNull) {
    BSONObj operand = BSON("$gte" << BSONNULL);
    GTEMatchExpression gte("a", operand["$gte"]);
    ASSERT(gte.matchesBSON(BSONObj(), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!gte.matchesBSON(BSON("a" << 4), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(gte.matchesBSON(BSON("b" << 4), NULL));
}