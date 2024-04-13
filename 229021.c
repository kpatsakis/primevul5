TEST(LtOp, MatchesMaxKey) {
    BSONObj operand = BSON("a" << MaxKey);
    LTMatchExpression lt("a", operand["a"]);
    BSONObj minKeyObj = BSON("a" << MinKey);
    BSONObj maxKeyObj = BSON("a" << MaxKey);
    BSONObj numObj = BSON("a" << 4);

    ASSERT(lt.matchesBSON(minKeyObj, NULL));
    ASSERT(!lt.matchesBSON(maxKeyObj, NULL));
    ASSERT(lt.matchesBSON(numObj, NULL));

    ASSERT(lt.matchesSingleElement(minKeyObj.firstElement()));
    ASSERT(!lt.matchesSingleElement(maxKeyObj.firstElement()));
    ASSERT(lt.matchesSingleElement(numObj.firstElement()));
}