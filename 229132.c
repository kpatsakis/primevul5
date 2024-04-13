TEST(LteOp, MatchesElement) {
    BSONObj operand = BSON("$lte" << 5);
    BSONObj match = BSON("a" << 4.5);
    BSONObj equalMatch = BSON("a" << 5);
    BSONObj notMatch = BSON("a" << 6);
    BSONObj notMatchWrongType = BSON("a"
                                     << "foo");
    LTEMatchExpression lte("", operand["$lte"]);
    ASSERT(lte.matchesSingleElement(match.firstElement()));
    ASSERT(lte.matchesSingleElement(equalMatch.firstElement()));
    ASSERT(!lte.matchesSingleElement(notMatch.firstElement()));
    ASSERT(!lte.matchesSingleElement(notMatchWrongType.firstElement()));
}