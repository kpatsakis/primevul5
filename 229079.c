TEST(EqOp, MatchesNestedNull) {
    BSONObj operand = BSON("a.b" << BSONNULL);
    EqualityMatchExpression eq("a.b", operand["a.b"]);
    // null matches any empty object that is on a subpath of a.b
    ASSERT(eq.matchesBSON(BSONObj(), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSONObj()), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSON_ARRAY(BSONObj())), NULL));
    ASSERT(eq.matchesBSON(BSON("a" << BSON("b" << BSONNULL)), NULL));
    // b does not exist as an element in array under a.
    ASSERT(!eq.matchesBSON(BSON("a" << BSONArray()), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(BSONNULL)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2)), NULL));
    // a.b exists but is not null.
    ASSERT(!eq.matchesBSON(BSON("a" << BSON("b" << 4)), NULL));
    ASSERT(!eq.matchesBSON(BSON("a" << BSON("b" << BSONObj())), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(eq.matchesBSON(BSON("b" << 4), NULL));
}