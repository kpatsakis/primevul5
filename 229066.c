TEST(EqOp, MatchesReferencedObjectValue) {
    BSONObj operand = BSON("a.b" << 5);
    EqualityMatchExpression eq("a.b", operand["a.b"]);
    ASSERT(eq.matchesBSON(BSON("a" << BSON("b" << 5)), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSON("b" << BSON_ARRAY(5))), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSON_ARRAY(BSON("b" << 5))), NULL));
}