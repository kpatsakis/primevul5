TEST(GteOp, MatchesWholeArray) {
    BSONObj operand = BSON("$gte" << BSON_ARRAY(5));
    GTEMatchExpression gte("a", operand["$gte"]);
    ASSERT(!gte.matchesBSON(BSON("a" << BSON_ARRAY(4)), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(5)), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(6)), NULL));
    // Nested array.
    // XXX: The following assertion documents current behavior.
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(4))), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(5))), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(6))), NULL));
}