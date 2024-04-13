TEST(LtOp, MatchesElement) {
    BSONObj operand = BSON("$lt" << 5);
    BSONObj match = BSON("a" << 4.5);
    BSONObj notMatch = BSON("a" << 6);
    BSONObj notMatchEqual = BSON("a" << 5);
    BSONObj notMatchWrongType = BSON("a"
                                     << "foo");
    LTMatchExpression lt("", operand["$lt"]);
    ASSERT(lt.matchesSingleElement(match.firstElement()));
    ASSERT(!lt.matchesSingleElement(notMatch.firstElement()));
    ASSERT(!lt.matchesSingleElement(notMatchEqual.firstElement()));
    ASSERT(!lt.matchesSingleElement(notMatchWrongType.firstElement()));
}