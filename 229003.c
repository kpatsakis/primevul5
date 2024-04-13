TEST(ModMatchExpression, MatchesElement) {
    BSONObj match = BSON("a" << 1);
    BSONObj largerMatch = BSON("a" << 4.0);
    BSONObj longLongMatch = BSON("a" << 68719476736LL);
    BSONObj notMatch = BSON("a" << 6);
    BSONObj negativeNotMatch = BSON("a" << -2);
    ModMatchExpression mod("", 3, 1);
    ASSERT(mod.matchesSingleElement(match.firstElement()));
    ASSERT(mod.matchesSingleElement(largerMatch.firstElement()));
    ASSERT(mod.matchesSingleElement(longLongMatch.firstElement()));
    ASSERT(!mod.matchesSingleElement(notMatch.firstElement()));
    ASSERT(!mod.matchesSingleElement(negativeNotMatch.firstElement()));
}