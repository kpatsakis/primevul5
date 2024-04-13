TEST(GteOp, MatchesDotNotationNull) {
    BSONObj operand = BSON("$gte" << BSONNULL);
    GTEMatchExpression gte("a.b", operand["$gte"]);
    ASSERT(gte.matchesBSON(BSONObj(), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << 4), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSONObj()), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("b" << BSONNULL))), NULL));
    ASSERT(gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("a" << 4) << BSON("b" << 4))), NULL));
    ASSERT(!gte.matchesBSON(BSON("a" << BSON_ARRAY(4)), NULL));
    ASSERT(!gte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("b" << 4))), NULL));
}