TEST(GtOp, MatchesMinKey) {
    BSONObj operand = BSON("a" << MinKey);
    GTMatchExpression gt("a", operand["a"]);
    BSONObj minKeyObj = BSON("a" << MinKey);
    BSONObj maxKeyObj = BSON("a" << MaxKey);
    BSONObj numObj = BSON("a" << 4);

    ASSERT(!gt.matchesBSON(minKeyObj, NULL));
    ASSERT(gt.matchesBSON(maxKeyObj, NULL));
    ASSERT(gt.matchesBSON(numObj, NULL));

    ASSERT(!gt.matchesSingleElement(minKeyObj.firstElement()));
    ASSERT(gt.matchesSingleElement(maxKeyObj.firstElement()));
    ASSERT(gt.matchesSingleElement(numObj.firstElement()));
}