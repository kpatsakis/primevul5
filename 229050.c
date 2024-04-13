TEST(InMatchExpression, ElemMatchKey) {
    BSONObj operand = BSON_ARRAY(5 << 2);
    InMatchExpression in("a");
    std::vector<BSONElement> equalities{operand[0], operand[1]};
    ASSERT_OK(in.setEqualities(std::move(equalities)));

    MatchDetails details;
    details.requestElemMatchKey();
    ASSERT(!in.matchesBSON(BSON("a" << 4), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(in.matchesBSON(BSON("a" << 5), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(in.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2 << 5)), &details));
    ASSERT(details.hasElemMatchKey());
    ASSERT_EQUALS("1", details.elemMatchKey());
}