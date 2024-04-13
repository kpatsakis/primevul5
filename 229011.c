TEST(ExistsMatchExpression, ElemMatchKey) {
    ExistsMatchExpression exists("a.b");
    MatchDetails details;
    details.requestElemMatchKey();
    ASSERT(!exists.matchesBSON(BSON("a" << 1), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(exists.matchesBSON(BSON("a" << BSON("b" << 6)), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(exists.matchesBSON(BSON("a" << BSON_ARRAY(2 << BSON("b" << 7))), &details));
    ASSERT(details.hasElemMatchKey());
    ASSERT_EQUALS("1", details.elemMatchKey());
}