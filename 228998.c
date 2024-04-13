TEST(EqOp, MatchesElement) {
    BSONObj operand = BSON("a" << 5);
    BSONObj match = BSON("a" << 5.0);
    BSONObj notMatch = BSON("a" << 6);

    EqualityMatchExpression eq("", operand["a"]);
    ASSERT(eq.matchesSingleElement(match.firstElement()));
    ASSERT(!eq.matchesSingleElement(notMatch.firstElement()));

    ASSERT(eq.equivalent(&eq));
}