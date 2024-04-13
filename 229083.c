TEST(GtOp, MatchesWholeArray) {
    BSONObj operand = BSON("$gt" << BSON_ARRAY(5));
    GTMatchExpression gt("a", operand["$gt"]);
    ASSERT(!gt.matchesBSON(BSON("a" << BSON_ARRAY(4)), NULL));
    ASSERT(!gt.matchesBSON(BSON("a" << BSON_ARRAY(5)), NULL));
    ASSERT(gt.matchesBSON(BSON("a" << BSON_ARRAY(6)), NULL));
    // Nested array.
    // XXX: The following assertion documents current behavior.
    ASSERT(gt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(4))), NULL));
    // XXX: The following assertion documents current behavior.
    ASSERT(gt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(5))), NULL));
    ASSERT(gt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(6))), NULL));
}