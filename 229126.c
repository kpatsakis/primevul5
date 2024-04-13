TEST(EqOp, MatchesReferencedArrayValue) {
    BSONObj operand = BSON("a.0" << 5);
    EqualityMatchExpression eq("a.0", operand["a.0"]);
    ASSERT(eq.matchesBSON(BSON("a" << BSON_ARRAY(5)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(5))), NULL));
}