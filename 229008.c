TEST(GteOp, MatchesMaxKey) {
    BSONObj operand = BSON("a" << MaxKey);
    GTEMatchExpression gte("a", operand["a"]);
    BSONObj minKeyObj = BSON("a" << MinKey);
    BSONObj maxKeyObj = BSON("a" << MaxKey);
    BSONObj numObj = BSON("a" << 4);

    ASSERT(!gte.matchesBSON(minKeyObj, NULL));
    ASSERT(gte.matchesBSON(maxKeyObj, NULL));
    ASSERT(!gte.matchesBSON(numObj, NULL));

    ASSERT(!gte.matchesSingleElement(minKeyObj.firstElement()));
    ASSERT(gte.matchesSingleElement(maxKeyObj.firstElement()));
    ASSERT(!gte.matchesSingleElement(numObj.firstElement()));
}