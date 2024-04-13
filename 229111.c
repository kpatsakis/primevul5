TEST(GteOp, MatchesElement) {
    BSONObj operand = BSON("$gte" << 5);
    BSONObj match = BSON("a" << 5.5);
    BSONObj equalMatch = BSON("a" << 5);
    BSONObj notMatch = BSON("a" << 4);
    BSONObj notMatchWrongType = BSON("a"
                                     << "foo");
    GTEMatchExpression gte("", operand["$gte"]);
    ASSERT(gte.matchesSingleElement(match.firstElement()));
    ASSERT(gte.matchesSingleElement(equalMatch.firstElement()));
    ASSERT(!gte.matchesSingleElement(notMatch.firstElement()));
    ASSERT(!gte.matchesSingleElement(notMatchWrongType.firstElement()));
}