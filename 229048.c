TEST(RegexMatchExpression, ElemMatchKey) {
    RegexMatchExpression regex("a", "b", "");
    MatchDetails details;
    details.requestElemMatchKey();
    ASSERT(!regex.matchesBSON(BSON("a"
                                   << "c"),
                              &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(regex.matchesBSON(BSON("a"
                                  << "b"),
                             &details));
    ASSERT(!details.hasElemMatchKey());
    ASSERT(regex.matchesBSON(BSON("a" << BSON_ARRAY("c"
                                                    << "b")),
                             &details));
    ASSERT(details.hasElemMatchKey());
    ASSERT_EQUALS("1", details.elemMatchKey());
}