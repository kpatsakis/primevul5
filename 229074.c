TEST(LteOp, MatchesMinKey) {
    BSONObj operand = BSON("a" << MinKey);
    LTEMatchExpression lte("a", operand["a"]);
    BSONObj minKeyObj = BSON("a" << MinKey);
    BSONObj maxKeyObj = BSON("a" << MaxKey);
    BSONObj numObj = BSON("a" << 4);

    ASSERT(lte.matchesBSON(minKeyObj, NULL));
    ASSERT(!lte.matchesBSON(maxKeyObj, NULL));
    ASSERT(!lte.matchesBSON(numObj, NULL));

    ASSERT(lte.matchesSingleElement(minKeyObj.firstElement()));
    ASSERT(!lte.matchesSingleElement(maxKeyObj.firstElement()));
    ASSERT(!lte.matchesSingleElement(numObj.firstElement()));
}