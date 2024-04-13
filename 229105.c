TEST(InMatchExpression, MatchesNull) {
    BSONObj operand = BSON_ARRAY(BSONNULL);

    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand.firstElement()};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    ASSERT(in.matchesBSON(BSONObj(), NULL));
    ASSERT(in.matchesBSON(BSON("a" << BSONNULL), NULL));
    ASSERT(!in.matchesBSON(BSON("a" << 4), NULL));
    // A non-existent field is treated same way as an empty bson object
    ASSERT(in.matchesBSON(BSON("b" << 4), NULL));
}