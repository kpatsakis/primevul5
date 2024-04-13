TEST(ModMatchExpression, ElemMatchKey) {
    ModMatchExpression mod("a", 5, 2);
    MatchDetails details;
    details.requestElemMatchKey();
    ASSERT(!mod.matchesBSON(BSON("a" << 4), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(mod.matchesBSON(BSON("a" << 2), &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(mod.matchesBSON(BSON("a" << BSON_ARRAY(1 << 2 << 5)), &details));
    ASSERT(details.hasElemMatchKey());
    ASSERT_EQUALS("1", details.elemMatchKey());
}