TEST(EqOp, MatchesMinKey) {
    BSONObj operand = BSON("a" << MinKey);
    EqualityMatchExpression eq("a", operand["a"]);
    BSONObj minKeyObj = BSON("a" << MinKey);
    BSONObj maxKeyObj = BSON("a" << MaxKey);
    BSONObj numObj = BSON("a" << 4);

    ASSERT(eq.matchesBSON(minKeyObj, NULL));
    ASSERT(!eq.matchesBSON(maxKeyObj, NULL));
    ASSERT(!eq.matchesBSON(numObj, NULL));

    ASSERT(eq.matchesSingleElement(minKeyObj.firstElement()));
    ASSERT(!eq.matchesSingleElement(maxKeyObj.firstElement()));
    ASSERT(!eq.matchesSingleElement(numObj.firstElement()));
}