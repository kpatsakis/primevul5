TEST(LteOp, MatchesDotNotationNull) {
    BSONObj operand = BSON("$lte" << BSONNULL);
    LTEMatchExpression lte("a.b", operand["$lte"]);
    ASSERT(lte.matchesBSON(BSONObj(), NULL));
    ASSERT(lte.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(lte.matchesBSON(BSON("a" << 4), NULL));
    ASSERT(lte.matchesBSON(BSON("a" << BSONObj()), NULL));
    ASSERT(lte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("b" << BSONNULL))), NULL));
    ASSERT(lte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("a" << 4) << BSON("b" << 4))), NULL));
    ASSERT(!lte.matchesBSON(BSON("a" << BSON_ARRAY(4)), NULL));
    ASSERT(!lte.matchesBSON(BSON("a" << BSON_ARRAY(BSON("b" << 4))), NULL));
}