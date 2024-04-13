TEST(ExistsMatchExpression, MatchesElement) {
    BSONObj existsInt = BSON("a" << 5);
    BSONObj existsNull = BSON("a" << BSONNULL);
    BSONObj doesntExist = BSONObj();
    ExistsMatchExpression exists("");
    ASSERT(exists.matchesSingleElement(existsInt.firstElement()));
    ASSERT(exists.matchesSingleElement(existsNull.firstElement()));
    ASSERT(!exists.matchesSingleElement(doesntExist.firstElement()));
}