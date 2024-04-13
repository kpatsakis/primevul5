TEST(InMatchExpression, MatchesArrayValue) {
    BSONObj operand = BSON_ARRAY(5);
    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    ASSERT(in.matchesBSON(BSON("a" << BSON_ARRAY(5.0 << 6)), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << BSON_ARRAY(6 << 7)), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << BSON_ARRAY(BSON_ARRAY(5))), NULL));
}