TEST(InMatchExpression, MatchesElementSingle) {
    BSONArray operand = BSON_ARRAY(1);
    BSONObj match = BSON("a" << 1);
    BSONObj notMatch = BSON("a" << 2);
    InMatchExpression in("");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));
    ASSERT(in.matchesSingleElement(match["a"]));
    ASSERT(!in.matchesSingleElement(notMatch["a"]));
}