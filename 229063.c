TEST(InMatchExpression, MatchesMinKey) {
    BSONObj operand = BSON_ARRAY(MinKey);
    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    ASSERT(in.matchesBSON(BSON("a" << MinKey), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << MaxKey), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << 4), NULL));
}