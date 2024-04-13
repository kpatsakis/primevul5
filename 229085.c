TEST(LtOp, MatchesWholeArray) {
    BSONObj operand = BSON("$lt" << BSON_ARRAY(5));
    LTMatchExpression lt("a", operand["$lt"]);
    ASSERT(lt.matchesBSON(BSON("a" << BSON_ARRAY(4)), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSON_ARRAY(5)), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSON_ARRAY(6)), NULL));
    // Nested array.
    ASSERT(lt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(4))), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(5))), NULL));
    ASSERT(!lt.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(6))), NULL));
}