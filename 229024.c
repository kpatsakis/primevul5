TEST(InMatchExpression, MatchesFullArray) {
    BSONObj operand = BSON_ARRAY(BSON_ARRAY(1 << 2) << 4 << 5);
    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand[0], operand[1], operand[2]};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    ASSERT(in.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2)), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2 << 3)), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << BSON_ARRAY(1)), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << 1), NULL));
}