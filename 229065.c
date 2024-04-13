TEST(InMatchExpression, MatchesElementMultiple) {
    BSONObj operand = BSON_ARRAY(1 << "r" << true << 1);
    InMatchExpression in("");
    std::vector<BSONElement> equalities{operand[0], operand[1], operand[2], operand[3]};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    BSONObj matchFirst = BSON("a" << 1);
    BSONObj matchSecond = BSON("a"
                               << "r");
    BSONObj matchThird = BSON("a" << true);
    BSONObj notMatch = BSON("a" << false);
    ASSERT(in.matchesSingleElement(matchFirst["a"]));
    ASSERT(in.matchesSingleElement(matchSecond["a"]));
    ASSERT(in.matchesSingleElement(matchThird["a"]));
    ASSERT(!in.matchesSingleElement(notMatch["a"]));
}