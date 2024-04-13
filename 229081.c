TEST(InMatchExpression, MatchesUndefined) {
    BSONObj operand = BSON_ARRAY(BSONUndefined);

    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_NOT_OK(in.setEqualities(std::move(equalities)));
}