TEST(ExistsMatchExpression, MatchesElementExistsTrueValue) {
    BSONObj exists = BSON("a" << 5);
    BSONObj missing = BSONObj();
    ExistsMatchExpression existsTrueValue("");
    ASSERT(existsTrueValue.matchesSingleElement(exists.firstElement()));
    ASSERT(!existsTrueValue.matchesSingleElement(missing.firstElement()));
}