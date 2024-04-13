TEST(InMatchExpression, MatchesEmpty) {
    InMatchExpression in("a");

    BSONObj notMatch = BSON("a" << 2);
    ASSERT(!in.matchesSingleElement(notMatch["a"]));
    ASSERT(!in.matchesBSON(BSON("a" << 1), NULL));
    ASSERT(!in.matchesBSON(BSONObj(), NULL));
}